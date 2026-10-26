cmake_minimum_required(VERSION 3.22)
project(MeuAutoTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

# 1. Baixa a biblioteca fmt (dependência do Preloader para logs)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.0.2
)
FetchContent_MakeAvailable(fmt)

# 2. Baixa o SDK do Preloader
FetchContent_Declare(
    preloader_android
    GIT_REPOSITORY https://github.com/LiteLDev/preloader-android.git
    GIT_TAG 0.2.2
    SOURCE_SUBDIR cmake/preloader-sdk
)
FetchContent_MakeAvailable(preloader_android)

# 3. Cria a biblioteca do mod
add_library(MeuAutoTool SHARED src/main.cpp)

# 4. Define os caminhos de include do Preloader e do fmt
target_include_directories(MeuAutoTool PRIVATE 
    "${preloader_android_SOURCE_DIR}/include"
    "${fmt_SOURCE_DIR}/include"
)

# 5. Linka com o Preloader, fmt e a biblioteca de log do Android
target_link_libraries(MeuAutoTool PRIVATE preloader fmt::fmt log)

# 6. Flags de otimizacao e visibilidade
target_compile_options(MeuAutoTool PRIVATE -fvisibility=hidden -Oz -ffunction-sections -fdata-sections)
target_link_options(MeuAutoTool PRIVATE -Wl,--gc-sections -Wl,--strip-all)
