macro (build_and_run arg)
	add_executable(${arg} ${arg}.cpp)
	target_link_libraries (${arg} sqlpp11-connector-sqlite3)
	target_link_libraries (${arg} sqlite3)
	add_test(${arg} ${arg})
endmacro ()

build_and_run(SampleTest)
build_and_run(SelectTest)
