# - Find mysqlclient
# Find the native sqlite3 includes and library
#
#  SQLITE3_INCLUDE_DIRS - where to find mysql.h, etc.
#  SQLITE3_LIBRARIES    - List of libraries when using sqlite3.
#  SQLITE3_FOUND        - True if sqlite3 found.

if (NOT DEFINED MSVC)
	find_path(SQLITE3_INCLUDE_DIR
		NAMES sqlite3.h
		)

	find_library(SQLITE3_LIBRARY
		NAMES sqlite3
		)
else()
	message("CMAKE_PREFIX_PATH: " ${CMAKE_PREFIX_PATH})
	if (EXISTS "${CMAKE_PREFIX_PATH}/sqlite3.h")
		message("found the header")
	else()
		message("header not found")
	endif()
	if (EXISTS "${CMAKE_PREFIX_PATH}/sqlite3.lib")
		message("found the library")
	else()
		message("library not found")
	endif()
	file(GLOB SQLITE3_GLOB_PROGRAM "$ENV{SystemDrive}/Program Files/Sqlite3*/*")
	file(GLOB SQLITE3_GLOB_PROGRAM86 "$ENV{SystemDrive}/Program Files (x86)/Sqlite3*/*")
	find_path(SQLITE3_INCLUDE_DIR
		NAMES sqlite3.h
		PATHS ${SQLITE3_GLOB_PROGRAM}
					${SQLITE3_GLOB_PROGRAM86}
					"C:/projects/sqlpp11-connector-sqlite3/sqlite-amalgamation-3120200"
		)

	find_library(SQLITE3_LIBRARY
		NAMES sqlite3
		PATHS ${SQLITE3_GLOB_PROGRAM}
					${SQLITE3_GLOB_PROGRAM86}
					"C:/projects/sqlpp11-connector-sqlite3/sqlite-amalgamation-3120200"
		)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLITE3
  FOUND_VAR SQLITE3_FOUND
  REQUIRED_VARS SQLITE3_LIBRARY SQLITE3_INCLUDE_DIR
  )

mark_as_advanced(
  SQLITE3_LIBRARY
  SQLITE3_INCLUDE_DIR
  )

set(SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
set(SQLITE3_LIBRARIES ${SQLITE3_LIBRARY})
