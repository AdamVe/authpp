add_executable(authpp main.cpp)
target_link_libraries(authpp PRIVATE authpp_lib ${LIBUSB_LIBRARY} fmt::fmt)
