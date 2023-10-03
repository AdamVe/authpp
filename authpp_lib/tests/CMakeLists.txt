FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCE_LIST
        byte_buffer_test.cpp
        oath_name_parser_test.cpp
)

add_executable(
        authpp_test
        ${TEST_SOURCE_LIST}
)

target_link_libraries(
        authpp_test
        GTest::gtest_main
        authpp_lib ${LIBUSB_LIBRARY} fmt::fmt
)

include(GoogleTest)
gtest_discover_tests(authpp_test)
