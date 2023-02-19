add_executable(authpp main.cc)
target_link_libraries(authpp PRIVATE authpp_lib ${LIBUSB_LIBRARY} fmt::fmt)
