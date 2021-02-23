cmake_minimum_required(VERSION 3.9)
project (main)

add_subdirectory(external/cpprestsdk)

include_directories(
  ${PROJECT_SOURCE_DIR}/external/cpprestsdk/Release/include/
) 

add_executable(${PROJECT_NAME} main.cpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

target_link_libraries(${PROJECT_NAME}
    cpprest
    )