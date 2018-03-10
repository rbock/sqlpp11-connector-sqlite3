#.rst:
# FindSqlpp11
# ---------------
#
# This module finds Sqlpp11 library
#
# This will define the following variables::
#
#   Sqlpp11_FOUND       - True if the system has the library
#   Sqlpp11_INCLUDE_DIR - The directory which includes the headers
#   Sqlpp11_ROOT_DIR    - The base directory of the library
#
# and the following imported targets::
#
#   sqlpp11::sqlpp11   - The target to use sqlpp11/sqlpp11.h
#
# You can set Sqlpp11_ROOT_DIR as hint to the location of the library.
#
# The target will enable the required C++11 standard in your compiler. You can
# use any later standard but you have to enable them explicitly.

cmake_minimum_required(VERSION 3.1)

# ensure cache entry
set(Sqlpp11_ROOT_DIR "${Sqlpp11_ROOT_DIR}" CACHE PATH "Root directory of sqlpp11 library")
set(Sqlpp11_NOT_FOUND_MESSAGE "Could NOT find Sqlpp11.
Maybe you need to adjust the search paths or Sqlpp11_ROOT_DIR.")

find_file(Sqlpp11_INCLUDE_FILE
    sqlpp11/sqlpp11.h include/sqlpp11/sqlpp11.h
    HINTS
      ${Sqlpp11_ROOT_DIR}
      ${Sqlpp11_ROOT_DIR}/include
)
mark_as_advanced(Sqlpp11_INCLUDE_FILE)

if (Sqlpp11_INCLUDE_FILE)
    # Validate that correct file is found
    file(STRINGS ${Sqlpp11_INCLUDE_FILE} check_result
        LIMIT_COUNT 1
        REGEX "^ *[\\*] Copyright [(]c[)] 2013-[0-9]+, Roland Bock *$"
    )

    if("${check_result}" STREQUAL "")
        string(APPEND Sqlpp11_NOT_FOUND_MESSAGE "\nRejecting found '${Sqlpp11_INCLUDE_FILE}', it seems to be a name twin.")
        unset(Sqlpp11_INCLUDE_FILE CACHE)
    else()
        # Check succeeded, create target
        get_filename_component(Sqlpp11_INCLUDE_DIR "${Sqlpp11_INCLUDE_FILE}" DIRECTORY) # remove filename
        get_filename_component(Sqlpp11_INCLUDE_DIR "${Sqlpp11_INCLUDE_DIR}" DIRECTORY) # remove date dir
        mark_as_advanced(Sqlpp11_INCLUDE_DIR)
        set(Sqlpp11_ROOT_DIR "${Sqlpp11_INCLUDE_DIR}")
        unset(Sqlpp11_NOT_FOUND_MESSAGE)

        if(NOT TARGET sqlpp11::sqlpp11)
            find_package(HinnantDate REQUIRED)
            add_library(sqlpp11::sqlpp11 INTERFACE IMPORTED)
            set_target_properties(sqlpp11::sqlpp11 PROPERTIES
              INTERFACE_INCLUDE_DIRECTORIES "${Sqlpp11_INCLUDE_DIR}"
              INTERFACE_LINK_LIBRARIES HinnantDate::Date
            )
        endif()
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sqlpp11
    REQUIRED_VARS Sqlpp11_ROOT_DIR Sqlpp11_INCLUDE_DIR
    FOUND_VAR Sqlpp11_FOUND
    FAIL_MESSAGE ${Sqlpp11_NOT_FOUND_MESSAGE}
)
