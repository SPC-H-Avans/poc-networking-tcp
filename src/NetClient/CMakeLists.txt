cmake_minimum_required(VERSION 3.23)

project(SimpleClient)

add_executable(${PROJECT_NAME} src/MMO_Client.cpp)


target_sources(SimpleClient PUBLIC ../NetServer/include/NetServer/MMO_Common.hpp);

target_link_libraries(${PROJECT_NAME}
        sub::NetCommon
        )