cmake_minimum_required(VERSION 3.23)

project(NetCommon)



add_library(${PROJECT_NAME} INTERFACE include/NetCommon/net_tsqueue.hpp include/NetCommon/net_connection.hpp include/NetCommon/net_server.hpp include/NetCommon/net_client.hpp)
add_library(sub::NetCommon ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        INTERFACE
        ${PROJECT_SOURCE_DIR}/include/NetCommon
        )