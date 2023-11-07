# For gtest

#cmake_minimum_required(VERSION 3.14)
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

# To find intermediary
set(SOURCES
	../Capybara/intermediary.cpp
)

enable_testing()

add_executable(
  Unittest
  Unittest.cc
  ../Capybara/intermediary.cpp
)

target_link_libraries(
  Unittest
  GTest::gtest_main
  GTest::gmock_main
  jsoncpp_lib
  SQLite::SQLite3
)

include(GoogleTest)
gtest_discover_tests(Unittest)