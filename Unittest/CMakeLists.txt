# For gtest

cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(Unittest)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include jsoncpp
find_package(SQLite3 REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(nlohmann_json REQUIRED)

# To find intermediary
set(SOURCES
	../Capybara/intermediary.cpp
)

enable_testing()

add_executable(
  Unittest
  Unittest.cc
  ../Capybara/intermediary.cpp
  ../Capybara/database.cpp
)

target_link_libraries(
  Unittest
  GTest::gtest_main
  GTest::gmock_main
  jsoncpp_lib
  SQLite::SQLite3
  nlohmann_json::nlohmann_json
)

include(GoogleTest)
gtest_discover_tests(Unittest)