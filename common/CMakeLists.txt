set(SOURCE_LIST
  "src/common/arg_parser.cpp"
)

set(HEADER_LIST
  "include/common/arg_parser.h"
)

add_library(common_lib ${SOURCE_LIST})

target_include_directories(
  common_lib
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include/common"
)
