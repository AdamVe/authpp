cmake_minimum_required(VERSION 3.24)
project(authpp
  VERSION 0.1.0
  DESCRIPTION "Test"
  LANGUAGES CXX
)

if(AUTHPP_BUILD_TESTING)
  enable_testing()
endif()

add_executable(authpp
  src/CcidConnection.cpp
  src/main.cpp
  src/Message.cpp
  src/OathSession.cpp
  src/UsbDevice.cpp
  src/UsbConnection.cpp
  src/Util.cpp

  include/Apdu.h
  include/ByteArray.h
  include/CcidConnection.h
  include/Formatters.h
  include/Logger.h
  include/Message.h
  include/OathSession.h
  include/UsbDevice.h
  include/UsbConnection.h
  include/Util.h
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
