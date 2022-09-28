cmake_minimum_required(VERSION 3.23)

project(NetworkingPlayground)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

include_directories(asio INTERFACE "${CMAKE_SOURCE_DIR}/libs/asio-1.24.0/include")
message("CMAKE_SOURCE_DIR          ${CMAKE_SOURCE_DIR}/libs/asio-1.24.0/include")

if (WIN32)
    link_libraries(wsock32 ws2_32)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

add_subdirectory(libs/NetCommon)
add_subdirectory(src/NetServer)
add_subdirectory(src/NetClient)
add_subdirectory(src/Playground) # Executable (depends on A and C)