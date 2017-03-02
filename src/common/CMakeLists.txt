file(GLOB COMMON_SOURCE *.cpp *.h *.hpp)
file(GLOB GLTF_SOURCE gltf/*.hpp)
add_library(common ${COMMON_SOURCE} ${GLTF_SOURCE})
set_target_properties(common PROPERTIES FOLDER "common")