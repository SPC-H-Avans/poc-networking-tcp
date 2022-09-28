#pragma once

#ifndef MAIN_CPP_NET_COMMON_HPP
#define MAIN_CPP_NET_COMMON_HPP

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>

#ifdef _WIN32
    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0A00
    #endif
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

class net_common {

};


#endif //MAIN_CPP_NET_COMMON_HPP
