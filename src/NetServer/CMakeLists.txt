cmake_minimum_required(VERSION 3.23)

project(SimpleServer)

add_executable(${PROJECT_NAME} src/MMO_Server.cpp include/NetServer/MMO_Common.hpp)

target_link_libraries(${PROJECT_NAME}
        sub::NetCommon
        )