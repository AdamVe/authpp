
set(SOURCE_LIST
        "src/apdu.cpp"
        "src/byte_buffer.cpp"
        "src/ccid_connection.cpp"
        "src/message.cpp"
        "src/oath_session.cpp"
        "src/oath_session_helper.cpp"
        "src/time_util.cpp"
        "src/usb_device.cpp"
        "src/usb_manager.cpp"
        "src/util.cpp"
)

set(HEADER_LIST
        "include/libauthpp/apdu.h"
        "include/libauthpp/byte_buffer.h"
        "include/libauthpp/ccid_connection.h"
        "include/libauthpp/logger.h"
        "include/libauthpp/message.h"
        "include/libauthpp/oath_session.h"
        "include/libauthpp/oath_session_helper.h"
        "include/libauthpp/time_util.h"
        "include/libauthpp/usb_device.h"
        "include/libauthpp/usb_manager.h"
        "include/libauthpp/util.h"
        "include/libauthpp/oath_name_parser.h"
        "include/libauthpp/oath_name_parser.tpp"
        "include/libauthpp/fmt/fmt_byte_buffer.h"
        "include/libauthpp/fmt/fmt_logger.h"
        "include/libauthpp/fmt/fmt_message.h"
        "include/libauthpp/fmt/fmt_oath.h"
)

add_library(
        authpp_lib
        ${SOURCE_LIST}
        ${HEADER_LIST}
)

target_include_directories(authpp_lib
        PUBLIC
        "${fmtlib_SOURCE_DIR}/include/"
        "${libusb_INCLUDEDIR}/"
        "${OPENSSL_INCLUDE_DIR}/"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/libauthpp"
        INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include")

source_group(
        TREE "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PREFIX "Header files"
        FILES ${HEADER_LIST})
