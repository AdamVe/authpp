

pkg_check_modules(GTKMM gtkmm-4.0)

add_executable(authppgtk
  main.cpp
  main_window.cpp
  account_widget.cpp
)

target_include_directories(authppgtk PUBLIC
  "${authpp_SOURCE_DIR}/include"
  "${fmtlib_SOURCE_DIR}/include"
  "${OPENSSL_INCLUDE_DIR}"
  "${GTKMM_INCLUDE_DIRS}"
  "${libusb_INCLUDEDIR}/}")

target_link_directories(authppgtk
  PRIVATE
  ${GTKMM_LIBRARY_DIRS}
)

target_link_libraries(authppgtk
  PRIVATE
  authpp_lib
  ${GTKMM_LIBRARIES}
  ${libusb_LINK_LIBRARIES}
  fmt::fmt
  OpenSSL::SSL)
