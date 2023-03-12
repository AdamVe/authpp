
set(SOURCE_LIST
  "${authpp_SOURCE_DIR}/src/apdu.cpp"
  "${authpp_SOURCE_DIR}/src/byte_buffer.cpp"
  "${authpp_SOURCE_DIR}/src/ccid_connection.cpp"
  "${authpp_SOURCE_DIR}/src/message.cpp"
  "${authpp_SOURCE_DIR}/src/oath_session.cpp"
  "${authpp_SOURCE_DIR}/src/time_util.cpp"
  "${authpp_SOURCE_DIR}/src/usb_device.cpp"
  "${authpp_SOURCE_DIR}/src/usb_manager.cpp"
  "${authpp_SOURCE_DIR}/src/util.cpp"
  )

set(HEADER_LIST
  "${authpp_SOURCE_DIR}/include/apdu.h"
  "${authpp_SOURCE_DIR}/include/byte_buffer.h"
  "${authpp_SOURCE_DIR}/include/ccid_connection.h"
  "${authpp_SOURCE_DIR}/include/logger.h"
  "${authpp_SOURCE_DIR}/include/message.h"
  "${authpp_SOURCE_DIR}/include/oath_session.h"
  "${authpp_SOURCE_DIR}/include/time_util.h"
  "${authpp_SOURCE_DIR}/include/usb_device.h"
  "${authpp_SOURCE_DIR}/include/usb_manager.h"
  "${authpp_SOURCE_DIR}/include/util.h"
  "${authpp_SOURCE_DIR}/include/fmt/fmt_byte_buffer.h"
  "${authpp_SOURCE_DIR}/include/fmt/fmt_logger.h"
  "${authpp_SOURCE_DIR}/include/fmt/fmt_message.h"
  "${authpp_SOURCE_DIR}/include/fmt/fmt_oath.h"
  )

add_library(
  authpp_lib
  ${SOURCE_LIST}
  ${HEADER_LIST})

target_include_directories(authpp_lib PUBLIC
  "${authpp_SOURCE_DIR}/include"
  "${fmtlib_SOURCE_DIR}/include"
  "${OPENSSL_INCLUDE_DIR}"
  "${LIBUSB_INCLUDE_DIR}/../")

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header files"
  FILES ${HEADER_LIST})
