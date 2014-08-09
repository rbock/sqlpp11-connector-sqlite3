macro (build_and_run arg)
	# Add headers to sources to enable file browsing in IDEs
	add_executable(Sqlite3${arg} ${arg}.cpp ${sqlpp_headers})
	target_link_libraries (Sqlite3${arg} sqlpp11-connector-sqlite3)
	target_link_libraries (Sqlite3${arg} sqlite3)
	add_test(Sqlite3${arg} Sqlite3${arg})
endmacro ()

build_and_run(SampleTest)
build_and_run(SelectTest)
build_and_run(DynamicSelectTest)
