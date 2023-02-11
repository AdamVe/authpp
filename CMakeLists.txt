cmake_minimum_required(VERSION 3.24)
project(authpp
  VERSION 0.1.0
  DESCRIPTION "Auth++"
  LANGUAGES CXX
)

if(AUTHPP_BUILD_TESTING)
  enable_testing()
endif()

add_executable(authpp
  src/ccid_connection.cc
  src/main.cc
  src/message.cc
  src/oath_session.cc
  src/usb_device.cc
  src/util.cc

  include/apdu.h
  include/byte_array.h
  include/ccid_connection.h
  include/logger.h
  include/message.h
  include/oath_session.h
  include/usb_device.h
  include/util.h

  include/fmt/fmt_byte_array.h
  include/fmt/fmt_logger.h
  include/fmt/fmt_message.h
  include/fmt/fmt_oath_version.h
)

find_path(LIBUSB_INCLUDE_DIR
  NAMES libusb.h
  PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_library(LIBUSB_LIBRARY
  NAMES "usb-1.0"
  PATH_SUFFIXES "lib" "lib32" "lib64")

find_path(LIBFMT_INCLUDE_DIR
  NAMES format.h
  PAH_SUFFIXES "include" "fmt")
find_library(LIBFMT_LIBRARY
  NAMES fmt
  PATH_SUFFIXES "lib" "lib32" "lib64")

target_compile_features(authpp PUBLIC cxx_std_23)
target_compile_options(authpp PUBLIC -Wall)
target_include_directories(authpp PUBLIC include ${LIBUSB_INCLUDE_DIR} ${LIBFMT_INCLUDE_DIR})
target_link_libraries(authpp PUBLIC ${LIBUSB_LIBRARY} ${LIBFMT_LIBRARY})
