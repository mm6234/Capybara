# CMakeList.txt : CMake project for Capybara, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Capybara "Capybara.cpp" "Capybara.h")
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(Capybara PRIVATE Drogon::Drogon)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Capybara PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  hello_test
  hello_test.cc
)
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(
  hello_test PRIVATE Drogon::Drogon
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)