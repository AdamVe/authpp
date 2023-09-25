project(authppgtk VERSION 1.0.0)
set(VERSION_PRE_RELEASE_LABEL "SNAPSHOT")

pkg_check_modules(GTKMM gtkmm-4.0)
find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)

set(GRESOURCE_C org.adamve.authppgtk.gresource.c)
set(GRESOURCE_XML org.adamve.authppgtk.gresource.xml)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
        COMMAND ${GLIB_COMPILE_RESOURCES}
        ARGS
        ${GRESOURCE_XML}
        --target=${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
        --generate-source
        VERBATIM
        MAIN_DEPENDENCY data/${GRESOURCE_XML}
        DEPENDS
        data/ui/authppgtk.ui
        data/ui/account_widget.ui
)

add_custom_target(
        gtk-custom-resources
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
)

set(SOURCE_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/account_holder.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/app_window.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/worker.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}"
)

set(HEADER_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/include/account_holder.h"
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
        ${HEADER_LIST}
)

set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
        PROPERTIES GENERATED TRUE
)

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

add_dependencies(authppgtk gtk-custom-resources)

