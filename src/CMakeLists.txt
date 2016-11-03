set(CMAKE_CXX_VISIBILITY_PRESET hidden)
SET(USE_ASM    TRUE CACHE BOOL "enable/disable assembler optimizations")

add_subdirectory(Input)
add_subdirectory(Effect)
add_subdirectory(Visual)
