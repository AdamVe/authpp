
set(SOURCE_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/src/apdu.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/byte_buffer.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ccid_connection.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/message.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/oath_session.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/oath_session_helper.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/time_util.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/usb_device.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/usb_manager.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp"
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
