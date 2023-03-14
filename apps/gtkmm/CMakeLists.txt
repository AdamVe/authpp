
find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-4.0)

add_executable(authppgtk
  main.cpp
  main_window.cpp
)

target_include_directories(authppgtk PUBLIC
  "${authpp_SOURCE_DIR}/include"
  "${fmtlib_SOURCE_DIR}/include"
  "${OPENSSL_INCLUDE_DIR}"
  "${GTKMM_INCLUDE_DIRS}"
  "${LIBUSB_INCLUDE_DIR}/../}")

target_link_libraries(authppgtk
  PRIVATE
  authpp_lib
  ${GTKMM_LIBRARIES}
  ${LIBUSB_LIBRARY}
  fmt::fmt
  OpenSSL::SSL)