pkg_check_modules(GTKMM gtkmm-4.10)

set(SOURCE_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gtk_app.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/resources.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/account_holder.cpp")

set(HEADER_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/include/account_holder.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/gtk_app.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/resources.h")

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
)

add_dependencies(authppgtk copy-resources)

