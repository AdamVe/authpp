add_executable(authpp main.cpp)

target_include_directories(
        authpp
        PRIVATE
        ${libusb_INCLUDEDIR}
)

target_link_libraries(
        authpp
        PRIVATE
        authpp_lib
        common_lib
        ${libusb_LINK_LIBRARIES}
        fmt::fmt
        OpenSSL::SSL)
