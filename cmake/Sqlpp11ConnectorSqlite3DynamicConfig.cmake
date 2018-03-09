list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

include(CMakeFindDependencyMacro)
find_dependency(Sqlpp11 REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/Sqlpp11ConnectorSqlite3DynamicTargets.cmake")
