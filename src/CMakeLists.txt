set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_library(sqlpp11-connector-sqlite3
		connection.cpp
		bind_result.cpp
		prepared_statement.cpp
		detail/connection_handle.cpp
      )
add_library(sqlpp11::sqlite3 ALIAS sqlpp11-connector-sqlite3)

if (SQLPP_DYNAMIC_LOADING)
    add_library(sqlpp11-connector-sqlite3-dynamic
                    connection.cpp
                    bind_result.cpp
                    prepared_statement.cpp
                    detail/connection_handle.cpp
                    detail/dynamic_libsqlite3.cpp
        )
    add_library(sqlpp11::sqlite3-dynamic ALIAS sqlpp11-connector-sqlite3-dynamic)

    get_filename_component(SQLite3_LIB_FILE ${SQLite3_LIBRARY} NAME)

    if (WIN32)
        # the import library (.lib) needs to be replaced by .dll
        get_filename_component(SQLite3_LIB_FILE ${SQLite3_LIB_FILE} NAME_WE)
        string(APPEND SQLite3_LIB_FILE ".dll")
    endif()

    set_target_properties(sqlpp11-connector-sqlite3 PROPERTIES DEFINE_SYMBOL SQLPP11_COMPILING_DLL)
    set_target_properties(sqlpp11-connector-sqlite3-dynamic PROPERTIES DEFINE_SYMBOL SQLPP11_COMPILING_DLL)

    target_compile_definitions(sqlpp11-connector-sqlite3-dynamic
                               PUBLIC SQLPP_DYNAMIC_LOADING
                               PRIVATE SQLPP_DYNAMIC_LOADING_FILENAME=${SQLite3_LIB_FILE})

    target_include_directories(sqlpp11-connector-sqlite3-dynamic PUBLIC
                               $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                               $<INSTALL_INTERFACE:include>)

    target_link_libraries(sqlpp11-connector-sqlite3-dynamic PUBLIC sqlpp11:sqlpp11)
endif()

target_include_directories(sqlpp11-connector-sqlite3 PUBLIC sqlpp11:sqlpp11
                           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)

if (SQLCIPHER)
    target_compile_definitions(sqlpp11-connector-sqlite3 PUBLIC SQLPP_USE_SQLCIPHER)
    target_link_libraries(sqlpp11-connector-sqlite3 PUBLIC SQLCipher::SQLCipher)
    if (SQLPP_DYNAMIC_LOADING)
        target_compile_definitions(sqlpp11-connector-sqlite3-dynamic PUBLIC SQLPP_USE_SQLCIPHER)
        target_include_directories(sqlpp11-connector-sqlite3-dynamic PUBLIC ${SQLCIPHER_INCLUDE_DIRS})
    endif()
else()
    target_link_libraries(sqlpp11-connector-sqlite3 PUBLIC SQLite::SQLite3)
endif()

install(TARGETS sqlpp11-connector-sqlite3
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

if (SQLPP_DYNAMIC_LOADING)
    install(TARGETS sqlpp11-connector-sqlite3-dynamic
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)
endif()
