# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("example")

# Include sub-projects.
add_subdirectory ("example")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(MySQL REQUIRED)
#find_package(mysql-connector-c++ REQUIRED)
#find_package(Boost REQUIRED COMPONENTS system)

set(FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR "specify_path_to_mysqlconnector")
# The below three lines aren't utilized
include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include)
include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include/jdbc/cppconn)
link_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/lib64)

add_executable(MyExecutable ${CMAKE_CURRENT_SOURCE_DIR}/DatabaseCMake/example.cpp)

target_include_directories(MyExecutable PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
#target_link_libraries(MyExectuable PRIVATE ${MYSQL_CONNECTOR_CPP_LIBRARIES})

target_link_libraries(MyExecutable ${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/lib64)