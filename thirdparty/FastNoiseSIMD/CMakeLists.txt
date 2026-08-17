cmake_minimum_required(VERSION 3.16)

project(fastNoiseSIMD)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD/*.cpp") #只有 MY_SOURCES 为变量
add_library(fastNoiseSIMD)  ## fastNoiseSIMD 为一个库

target_sources(fastNoiseSIMD PRIVATE ${MY_SOURCES})
target_include_directories(fastNoiseSIMD PUBLIC "FastNoiseSIMD") ## 当将"FastNoiseSIMD"下的头文件包含进 fastNoiseSIMD 中

# 让 FastNoiseSIMD_avx2.cpp 使用 AVX2
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD/FastNoiseSIMD_avx2.cpp"
    PROPERTIES
    COMPILE_OPTIONS "/arch:AVX2"
)  