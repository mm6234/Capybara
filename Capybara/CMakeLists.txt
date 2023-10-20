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