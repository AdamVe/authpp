pkg_check_modules(GTKMM gtkmm-4.0)

add_executable(authppgtk
        src/main.cpp
        src/gtk_app.cpp
        src/account_widget.cpp
        src/resources.cpp
)

add_custom_command(
        TARGET authppgtk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/authppgtk.ui
        $<TARGET_FILE_DIR:authppgtk>/data/ui/authppgtk.ui
)

target_include_directories(authppgtk
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
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


