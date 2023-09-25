project(authppgtk VERSION 1.0.0)

set(VERSION_PRE_RELEASE_LABEL "SNAPSHOT")

set(SOURCE_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/resources.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/account_holder.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/app_window.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/worker.cpp"
)

set(HEADER_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/include/account_holder.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/resources.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/application.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/app_window.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/worker.h"
)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h")

add_executable(
        authppgtk
        ${SOURCE_LIST}
        ${HEADER_LIST})

target_include_directories(authppgtk
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        authpp_lib
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

add_custom_target(copy-resources ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:authppgtk>/data/ui/
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/authppgtk.ui" $<TARGET_FILE_DIR:authppgtk>/data/ui/authppgtk.ui
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/account_widget.ui" $<TARGET_FILE_DIR:authppgtk>/data/ui/account_widget.ui
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/authppgtk.css" $<TARGET_FILE_DIR:authppgtk>/data/ui/authppgtk.css
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/org.adamve.authppgtk.svg" $<TARGET_FILE_DIR:authppgtk>/data/ui/org.adamve.authppgtk.svg
)

pkg_check_modules(GTKMM gtkmm-4.0)
add_dependencies(authppgtk copy-resources)

