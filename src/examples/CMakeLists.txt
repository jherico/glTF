macro(BUILD_EXAMPLES)
    foreach(_FOLDER_NAME ${ARGN})
        message("Folder ${_FOLDER_NAME}")
        file(GLOB EXAMPLES ${_FOLDER_NAME}/*.cpp)
        foreach(EXAMPLE ${EXAMPLES})
            set(COMPILED_SHADERS "")
            get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)
            set(TARGET ${EXAMPLE_NAME}) 
            set(TARGET_NAME ${EXAMPLE_NAME})
            # Find any shaders
            string(REGEX REPLACE "^.._" "" EXAMPLE_BASE_NAME ${EXAMPLE_NAME})
            set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../data/shaders/${EXAMPLE_BASE_NAME}")
            file(GLOB SHADERS 
                ${SHADER_DIR}/*.vert 
                ${SHADER_DIR}/*.frag 
                ${SHADER_DIR}/*.comp 
                ${SHADER_DIR}/*.tesc 
                ${SHADER_DIR}/*.tese
                ${SHADER_DIR}/*.geom
                ${CMAKE_CURRENT_SOURCE_DIR}/../data/shaders/base/*.vert
                ${CMAKE_CURRENT_SOURCE_DIR}/../data/shaders/base/*.frag
            )

            source_group("Shaders" FILES ${SHADERS})
            foreach(SHADER ${SHADERS})
                compile_spirv_shader(${SHADER})
                list(APPEND COMPILED_SHADERS ${COMPILE_SPIRV_SHADER_RETURN})
            endforeach()
            source_group("Shaders\\Compiled" FILES ${COMPILED_SHADERS})
            add_executable(${EXAMPLE_NAME} ${EXAMPLE} ${SHADERS} ${COMPILED_SHADERS})
            set_target_properties(${EXAMPLE_NAME} PROPERTIES FOLDER "examples/${_FOLDER_NAME}")
            
            add_dependencies(${EXAMPLE_NAME} common)
            target_link_libraries(${EXAMPLE_NAME} ${LIBS})
            if (NOT WIN32)
                target_link_libraries(${EXAMPLE_NAME} Threads::Threads)
            endif()
            target_opengl()
            target_glew()
        endforeach()
    endforeach()
endmacro()


MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

subdirlist(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
build_examples(${SUBDIRS})

