#pragma once

#ifndef MAIN_CPP_NET_MESSAGE_HPP
#define MAIN_CPP_NET_MESSAGE_HPP
#endif //MAIN_CPP_NET_MESSAGE_HPP

#include "net_common.hpp"

namespace olc {
    namespace net {
        // Message header is sent at the start of all messages.
        // The template allows use to use enum class to ensure that the messages are valid at compile time
        template <typename T>
        struct message_header
        {
            T id{};
            uint32_t size = 0;
        };

        template <typename T>
        struct message
        {
            message_header<T> header{};
            std::vector<uint8_t> body;

            size_t size () const
            {
                return body.size();
            }

            friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
            {
                os << "ID: " << int(msg.header.id) << " Size:" << msg.header.size;
                return os;
            }

            template<typename DataType>
            friend message<T>& operator << (message<T>& msg, const DataType& data) //IDK WHAT THIS DOES EXACTLY
            {
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

                size_t i = msg.body.size();

                msg.body.resize(msg.body.size() + sizeof(DataType));

                std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

                msg.header.size = msg.size();

                return msg;
            }

            template<typename DataType>
            friend message<T>& operator >> (message<T>& msg, DataType& data)
            {
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed out of vector");

                size_t i = msg.body.size() - sizeof(DataType);

                std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

                msg.body.resize(i);

                msg.header.size = msg.size();

                return msg;
            }
        };

        //Forward declare the connection
        template <typename T>
        class connection;


        template <typename T>
        struct owned_message
        {
            std::shared_ptr<connection<T>> remote = nullptr;
            message<T> msg;

            //string maker
            friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg)
            {
                os << msg.msg;
                return os;
            }
        };
    }
}