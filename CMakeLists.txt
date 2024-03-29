cmake_minimum_required(VERSION 3.24)
project(authpp
        VERSION 0.1.0
        DESCRIPTION "Auth++"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    include(CTest)
endif ()

include(FetchContent)

FetchContent_Declare(
        fmtlib
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.1.1)

FetchContent_MakeAvailable(fmtlib)

find_package(PkgConfig)
pkg_check_modules(libusb REQUIRED libusb-1.0)
find_package(OpenSSL REQUIRED)

add_subdirectory(common)
add_subdirectory(authpp_lib)
add_subdirectory(apps)
