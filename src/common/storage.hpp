#pragma once
//
//  Created by Bradley Austin Davis on 2016/02/17
//  Copyright 2013-2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <stdint.h>
#include <vector>
#include <memory>
#include <Windows.h>

namespace storage {
    class Storage;
    using StoragePointer = std::shared_ptr<const Storage>;
    class MemoryStorage;
    using MemoryStoragePointer = std::shared_ptr<const MemoryStorage>;
    class FileStorage;
    using FileStoragePointer = std::shared_ptr<const FileStorage>;
    class ViewStorage;
    using ViewStoragePointer = std::shared_ptr<const ViewStorage>;

    class Storage : public std::enable_shared_from_this<Storage> {
    public:
        virtual ~Storage() {}
        virtual const uint8_t* data() const = 0;
        virtual size_t size() const = 0;

        ViewStoragePointer createView(size_t size = 0, size_t offset = 0) const;
        FileStoragePointer toFileStorage(const std::string& filename) const;
        MemoryStoragePointer toMemoryStorage() const;

        // Aliases to prevent having to re-write a ton of code
        inline size_t getSize() const { return size(); }
        inline const uint8_t* readData() const { return data(); }
    };

    class ViewStorage : public Storage {
    public:
        ViewStorage(const storage::StoragePointer& owner, size_t size, const uint8_t* data) : _owner(owner), _size(size), _data(data) {}
        const uint8_t* data() const override { return _data; }
        size_t size() const override { return _size; }
    private:
        const storage::StoragePointer _owner;
        const size_t _size;
        const uint8_t* _data;
    };


    ViewStoragePointer Storage::createView(size_t viewSize, size_t offset) const {
        auto selfSize = size();
        if (0 == viewSize) {
            viewSize = selfSize;
        }
        if ((viewSize + offset) > selfSize) {
            throw std::runtime_error("Invalid mapping range");
        }
        return ViewStoragePointer(new ViewStorage(shared_from_this(), viewSize, data() + offset));
    }


    class MemoryStorage : public Storage {
    public:
        MemoryStorage(size_t size, const uint8_t* data = nullptr);
        const uint8_t* data() const override { return _data.data(); }
        uint8_t* data() { return _data.data(); }
        size_t size() const override { return _data.size(); }
    private:
        std::vector<uint8_t> _data;
    };

    MemoryStorage::MemoryStorage(size_t size, const uint8_t* data) {
        _data.resize(size);
        if (data) {
            memcpy(_data.data(), data, size);
        }
    }

    class FileStorage : public Storage {
    public:
        static FileStoragePointer create(const std::string& filename, size_t size, const uint8_t* data);
        FileStorage(const std::string& filename);
        ~FileStorage();
        // Prevent copying
        FileStorage(const FileStorage& other) = delete;
        FileStorage& operator=(const FileStorage& other) = delete;

        const uint8_t* data() const override { return _mapped; }
        size_t size() const override { return _size; }

    private:
        HANDLE _file{ INVALID_HANDLE_VALUE };
        HANDLE _mapping{ INVALID_HANDLE_VALUE };
        uint64_t _size{ INVALID_FILE_SIZE };
        uint8_t* _mapped{ nullptr };
    };

    FileStoragePointer FileStorage::create(const std::string& filename, size_t size, const uint8_t* data) {
        auto newFile = CreateFile(filename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (newFile == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Unable to create file");
        }

        auto mapping = CreateFileMapping(newFile, 0, PAGE_READWRITE, (DWORD)(size >> 32), (DWORD)(size), nullptr);
        if (mapping == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Unable to map file");
        }
        auto dest = MapViewOfFile(mapping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
        if (!dest) {
            throw std::runtime_error("Unable to create map view of file");
        }
        memcpy(dest, data, size);
        UnmapViewOfFile(dest);
        CloseHandle(mapping);
        CloseHandle(newFile);
        return std::make_shared<FileStorage>(filename);
    }

    FileStorage::FileStorage(const std::string& filename) {
        _file = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if (_file == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Unable to create file");
        }
        
        DWORD fileSizeLow, fileSizeHigh;
        fileSizeLow = GetFileSize(_file, &fileSizeHigh);
        if (INVALID_FILE_SIZE == fileSizeLow) {
            throw std::runtime_error("Unable to fetch file size");
        }
        _size = fileSizeHigh;
        _size <<= 32;
        _size |= fileSizeLow;

        _mapping = CreateFileMapping(_file, 0, PAGE_READONLY, fileSizeHigh, fileSizeLow, 0);
        if (_mapping == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Unable to map file");
        }

        _mapped = static_cast<uint8_t*>(MapViewOfFile(_mapping, FILE_MAP_READ, 0, 0, 0));
        if (!_mapped) {
            throw std::runtime_error("Unable to create map view of file");
        }
    }

    FileStorage::~FileStorage() {
        if (_mapped) {
            UnmapViewOfFile(_mapped);
        }
        CloseHandle(_mapping);
        CloseHandle(_file);
    }

    MemoryStoragePointer Storage::toMemoryStorage() const {
        return MemoryStoragePointer(new MemoryStorage(size(), data()));
    }

    FileStoragePointer Storage::toFileStorage(const std::string& filename) const {
        return FileStorage::create(filename, size(), data());
    }

}
