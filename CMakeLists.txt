cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include("cmake/defaults.cmake")
set(NAME gltf)

project(${NAME})

add_custom_target(SetupRelease ALL ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(SetupRelease PROPERTIES FOLDER "CMakeTargets")
add_custom_target(SetupDebug ALL ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin_debug)
set_target_properties(SetupDebug PROPERTIES FOLDER "CMakeTargets")

if (WIN32)
    add_dependency_external_projects(glfw3)
    include_directories(${GLFW3_INCLUDE_DIR})
    list(APPEND EXTERNALS glfw3)
    list(APPEND LIBS ${GLFW3_LIBRARY}) 

    #add_dependency_external_projects(zlib)
    #include_directories(${ZLIB_INCLUDE_DIRS})
    #list(APPEND EXTERNALS zlib)
    #list(APPEND LIBS ${ZLIB_LIBRARIES}) 

    add_dependency_external_projects(glew)
    add_definitions(-DGLEW_STATIC)
    include_directories(${GLEW_INCLUDE_DIRS})
    list(APPEND EXTERNALS glew)
    list(APPEND LIBS ${GLEW_LIBRARY}) 
else()
    pkg_check_modules(GLFW3 REQUIRED glfw3>=3.2)
    include_directories(${GLFW3_INCLUDEDIR})
    link_directories(${GLFW3_LIBRARY_DIRS})
    list(APPEND LIBS ${GLFW3_LIBRARY}) 
endif()

add_dependency_external_projects(glm)
list(APPEND EXTERNALS glm)
include_directories(${GLM_INCLUDE_DIRS})

add_dependency_external_projects(gli)
list(APPEND EXTERNALS gli)
include_directories(${GLI_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/src/external/json/src)

add_subdirectory(src/common)
list(APPEND LIBS common) 
include_directories(${CMAKE_SOURCE_DIR}/src/common)
add_definitions(-DPROJECT_ROOT="${CMAKE_SOURCE_DIR}")

foreach(EXTERNAL ${EXTERNALS})
    add_dependencies(common ${EXTERNAL})
endforeach()

if (NOT WIN32)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif()

add_subdirectory(src/examples)

