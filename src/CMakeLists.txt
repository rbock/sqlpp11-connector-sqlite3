set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_library(sqlpp11-connector-sqlite3
		connection.cpp
		bind_result.cpp
		prepared_statement.cpp
		detail/connection_handle.cpp
      )

if (DYNAMIC_LOADING)
    add_library(sqlpp11-connector-sqlite3-dynamic
                    connection.cpp
                    bind_result.cpp
                    prepared_statement.cpp
                    detail/connection_handle.cpp
                    detail/dynamic_libsqlite3.cpp
        )

    get_filename_component(SQLITE3_LIB_FILE ${SQLITE3_LIBRARY} NAME)

    if (WIN32)
        # the import library (.lib) needs to be replaced by .dll
        get_filename_component(SQLITE3_LIB_FILE ${SQLITE3_LIB_FILE} NAME_WE)
        string(APPEND SQLITE3_LIB_FILE ".dll")
    endif()

    set_target_properties(sqlpp11-connector-sqlite3 PROPERTIES DEFINE_SYMBOL SQLPP11_COMPILING_DLL)
    set_target_properties(sqlpp11-connector-sqlite3-dynamic PROPERTIES DEFINE_SYMBOL SQLPP11_COMPILING_DLL)

    target_compile_definitions(sqlpp11-connector-sqlite3-dynamic
                               PUBLIC SQLPP_DYNAMIC_LOADING
                               PRIVATE SQLPP_DYNAMIC_LOADING_FILENAME=${SQLITE3_LIB_FILE})

    target_include_directories(sqlpp11-connector-sqlite3-dynamic
                               PUBLIC
                               ${SQLPP11_INCLUDE_DIR}
                               ${HinnantDate_INCLUDE_DIR}
															 ${SQLITE3_INCLUDE_DIRS}
                               $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                               $<INSTALL_INTERFACE:include>)
endif()
# TODO Once there is a Sqlpp11 target, add INTERFACE_INCLUDE_DIRECTORIES to sqlpp11
# TODO export Targets for HinnantDate and Sqlite3 as well
target_include_directories(sqlpp11-connector-sqlite3
                           PUBLIC
													 ${SQLITE3_INCLUDE_DIRS}
                           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(sqlpp11-connector-sqlite3 PUBLIC SQLPP11_SHARED)
    if (DYNAMIC_LOADING)
        target_compile_definitions(sqlpp11-connector-sqlite3-dynamic PUBLIC SQLPP11_SHARED)
    endif()
endif()

if (SQLCIPHER)
    target_compile_definitions(sqlpp11-connector-sqlite3 PUBLIC SQLPP_USE_SQLCIPHER)
    target_link_libraries(sqlpp11-connector-sqlite3 sqlpp11::sqlpp11 SQLCipher::SQLCipher)
    if (DYNAMIC_LOADING)
        target_compile_definitions(sqlpp11-connector-sqlite3-dynamic PUBLIC SQLPP_USE_SQLCIPHER)
        target_include_directories(sqlpp11-connector-sqlite3-dynamic PUBLIC ${SQLCIPHER_INCLUDE_DIRS})
    endif()
else()
    target_link_libraries(sqlpp11-connector-sqlite3 sqlpp11::sqlpp11 ${SQLITE3_LIBRARIES})
endif()

install(TARGETS sqlpp11-connector-sqlite3
        EXPORT Sqlpp11ConnectorSqlite3Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

include(CMakePackageConfigHelpers)

export(EXPORT Sqlpp11ConnectorSqlite3Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp11ConnectorSqlite3Targets.cmake"
)

configure_file(${PROJECT_SOURCE_DIR}/cmake/Sqlpp11ConnectorSqlite3Config.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp11ConnectorSqlite3Config.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/Sqlpp11ConnectorSqlite3)
install(EXPORT Sqlpp11ConnectorSqlite3Targets
  FILE
    Sqlpp11ConnectorSqlite3Targets.cmake
  NAMESPACE sqlpp11::
  DESTINATION
    ${ConfigPackageLocation}
)

install(
  FILES
    "${PROJECT_SOURCE_DIR}/cmake/Sqlpp11ConnectorSqlite3Config.cmake"
    "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSQLCipher.cmake"
    "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSqlite3.cmake"
  DESTINATION
    ${ConfigPackageLocation}
)

if (DYNAMIC_LOADING)
    install(TARGETS sqlpp11-connector-sqlite3-dynamic
            EXPORT Sqlpp11ConnectorSqlite3DynamicTargets
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)

    export(EXPORT Sqlpp11ConnectorSqlite3DynamicTargets
      FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp11ConnectorSqlite3DynamicTargets.cmake"
    )

    configure_file(${PROJECT_SOURCE_DIR}/cmake/Sqlpp11ConnectorSqlite3DynamicConfig.cmake
      "${CMAKE_CURRENT_BINARY_DIR}/cmake/Sqlpp11ConnectorSqlite3DynamicConfig.cmake"
      COPYONLY
    )

    set(ConfigPackageLocation lib/cmake/Sqlpp11ConnectorSqlite3Dynamic)
    install(EXPORT Sqlpp11ConnectorSqlite3DynamicTargets
      FILE
        Sqlpp11ConnectorSqlite3DynamicTargets.cmake
      NAMESPACE sqlpp11::
      DESTINATION
        ${ConfigPackageLocation}
    )

    install(
      FILES
        "${PROJECT_SOURCE_DIR}/cmake/Sqlpp11ConnectorSqlite3DynamicConfig.cmake"
        "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSQLCipher.cmake"
        "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSqlite3.cmake"
      DESTINATION
        ${ConfigPackageLocation}
    )

endif()
