FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.13.0)
FetchContent_MakeAvailable(googletest)

add_executable(
  authpp_test
  bytes_test.cc
)

target_link_libraries(
  authpp_test
  GTest::gtest_main
  authpp_lib ${LIBUSB_LIBRARY} fmt::fmt
)

include(GoogleTest)
gtest_discover_tests(authpp_test)
