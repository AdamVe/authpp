
set(SOURCE_LIST
  "${CMAKE_CURRENT_SOURCE_DIR}/src/apdu.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/byte_buffer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ccid_connection.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/message.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/oath_session.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/time_util.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/usb_device.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/usb_manager.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp"
  )

set(HEADER_LIST
  "${CMAKE_CURRENT_SOURCE_DIR}/include/apdu.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/byte_buffer.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ccid_connection.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/logger.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/message.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/oath_session.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/time_util.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/usb_device.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/usb_manager.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/util.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/fmt_byte_buffer.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/fmt_logger.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/fmt_message.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/fmt_oath.h"
  )

add_library(
  authpp_lib
  ${SOURCE_LIST}
  ${HEADER_LIST})

target_include_directories(authpp_lib
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${fmtlib_SOURCE_DIR}/include"
  "${OPENSSL_INCLUDE_DIR}"
  "${LIBUSB_INCLUDE_DIR}/../")

source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PREFIX "Header files"
  FILES ${HEADER_LIST})
