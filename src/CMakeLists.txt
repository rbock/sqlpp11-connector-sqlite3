set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_library(sqlpp11-connector-sqlite3
		connection.cpp
		bind_result.cpp
		prepared_statement.cpp
		detail/connection_handle.cpp
      )

add_library(sqlpp11-connector-sqlite3-dynamic
                connection.cpp
                bind_result.cpp
                prepared_statement.cpp
                detail/connection_handle.cpp
                detail/dynamic_libsqlite3.cpp
    )

get_filename_component(SQLITE3_LIB_FILE ${SQLITE3_LIBRARIES} NAME)

if (WIN32)
    # the import library (.lib) needs to be replaced by .dll
    get_filename_component(SQLITE3_LIB_FILE ${SQLITE3_LIB_FILE} NAME_WE)
    string(APPEND SQLITE3_LIB_FILE ".dll")
endif()

set_target_properties(sqlpp11-connector-sqlite3 PROPERTIES DEFINE_SYMBOL SQLPP11_COMPILING_DLL)
set_target_properties(sqlpp11-connector-sqlite3-dynamic PROPERTIES DEFINE_SYMBOL SQLPP11_COMPILING_DLL)

target_compile_definitions(sqlpp11-connector-sqlite3-dynamic PUBLIC SQLPP_DYNAMIC_LOADING)
target_compile_definitions(sqlpp11-connector-sqlite3-dynamic PRIVATE SQLPP_DYNAMIC_LOADING_FILENAME=${SQLITE3_LIB_FILE})

if (BUILD_SHARED_LIBS)
    target_compile_definitions(sqlpp11-connector-sqlite3 PUBLIC SQLPP11_SHARED)
    target_compile_definitions(sqlpp11-connector-sqlite3-dynamic PUBLIC SQLPP11_SHARED)
endif()

target_link_libraries(sqlpp11-connector-sqlite3 ${SQLITE3_LIBRARIES})

install(TARGETS sqlpp11-connector-sqlite3 sqlpp11-connector-sqlite3-dynamic
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
