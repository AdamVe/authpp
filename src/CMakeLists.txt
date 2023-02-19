
set(SOURCE_LIST
  "${authpp_SOURCE_DIR}/src/apdu.cc"
  "${authpp_SOURCE_DIR}/src/byte_buffer.cc"
  "${authpp_SOURCE_DIR}/src/ccid_connection.cc"
  "${authpp_SOURCE_DIR}/src/message.cc"
  "${authpp_SOURCE_DIR}/src/oath_session.cc"
  "${authpp_SOURCE_DIR}/src/usb_device.cc"
  "${authpp_SOURCE_DIR}/src/util.cc"
  )

set(HEADER_LIST
  "${authpp_SOURCE_DIR}/include/apdu.h"
  "${authpp_SOURCE_DIR}/include/byte_buffer.h"
  "${authpp_SOURCE_DIR}/include/ccid_connection.h"
  "${authpp_SOURCE_DIR}/include/logger.h"
  "${authpp_SOURCE_DIR}/include/message.h"
  "${authpp_SOURCE_DIR}/include/oath_session.h"
  "${authpp_SOURCE_DIR}/include/usb_device.h"
  "${authpp_SOURCE_DIR}/include/util.h"
  "${authpp_SOURCE_DIR}/include/fmt/fmt_byte_buffer.h"
  "${authpp_SOURCE_DIR}/include/fmt/fmt_logger.h"
  "${authpp_SOURCE_DIR}/include/fmt/fmt_message.h"
  "${authpp_SOURCE_DIR}/include/fmt/fmt_oath_version.h"
  )

add_library(
  authpp_lib
  ${SOURCE_LIST}
  ${HEADER_LIST})

target_include_directories(authpp_lib PUBLIC ../include)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header files"
  FILES ${HEADER_LIST})
