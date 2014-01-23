add_library(sqlpp11-connector-sqlite3
		connection.cpp
		char_result.cpp
		detail/connection_handle.cpp)

install(TARGETS sqlpp11-connector-sqlite3 DESTINATION lib)
