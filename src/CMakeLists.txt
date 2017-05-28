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
target_compile_options(sqlpp11-connector-sqlite3-dynamic INTERFACE -DSQLPP_DYNAMIC_LOADING)
target_compile_options(sqlpp11-connector-sqlite3-dynamic PRIVATE -DSQLPP_DYNAMIC_LOADING -DSQLPP_DYNAMIC_LOADING_FILENAME=${SQLITE3_LIB_FILE})

install(TARGETS sqlpp11-connector-sqlite3 DESTINATION lib)
install(TARGETS sqlpp11-connector-sqlite3-dynamic DESTINATION lib)
