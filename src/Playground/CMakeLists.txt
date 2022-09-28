cmake_minimum_required(VERSION 3.23)

project(Playground)

add_executable(${PROJECT_NAME} src/SimpleExample.cpp)

target_link_libraries(${PROJECT_NAME}
        sub::NetCommon
        )

