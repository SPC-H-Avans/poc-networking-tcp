#pragma once

#ifndef MAIN_CPP_NET_CONNECTION_HPP
#define MAIN_CPP_NET_CONNECTION_HPP
#endif //MAIN_CPP_NET_CONNECTION_HPP

#include "net_common.hpp"
#include "net_tsqueue.hpp"
#include "net_message.hpp"


namespace olc {
    namespace net {
        //forward declare
        template<typename T>
        class server_interface;

        template<typename T>
        class connection : public std::enable_shared_from_this<connection<T>> {
            public:
                enum class owner {
                    server,
                    client
                };

                connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
                    : m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
                {
                    m_nOwnerType = parent;

                    if(m_nOwnerType == owner::server) {
                        m_nHandshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());

                        m_nHandshakeCheck = scramble(m_nHandshakeOut);
                    }
                    else {
                        m_nHandshakeIn = 0;
                        m_nHandshakeOut = 0;
                    }
                }

                virtual ~connection() {}


                uint32_t GetID() const
                {
                    return id;
                }

                //Utilities
                void ConnectToClient(olc::net::server_interface<T>* server, uint32_t uid = 0) {
                    if(m_nOwnerType == owner::server) {
                        if(m_socket.is_open()) {
                            id = uid;
                            WriteValidation();
                            ReadValidation(server);
                        }
                    }
                }

                void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
                {
                    if(m_nOwnerType == owner::client)
                    {
                        asio::async_connect(m_socket, endpoints,
                            [this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
                            {
                                if(!ec) {
                                    ReadValidation();
                                }
                            });
                    }
                };

                void Disconnect() {
                    if (IsConnected())
                        asio::post(m_asioContext, [this]() { m_socket.close(); });
                }

                bool IsConnected() const {
                    return m_socket.is_open();
                };

                // Prime the connection to wait for incoming messages
                void StartListening()
                {

                }

                void Send(const message<T>& msg) {
                    asio::post(m_asioContext,
                       [this, msg]() {
                            bool bWritingMessage = !m_qMessagesOut.empty();
                            m_qMessagesOut.push_back(msg);

                            if(!bWritingMessage)
                                WriteHeader();
                        });
                }

            private:
                void ReadHeader() {
                    asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
                        [this](std::error_code ec, std::size_t length) {
                            if(!ec) {
                                if(m_msgTemporaryIn.header.size > 0) {
                                    m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
                                    ReadBody();
                                } else {
                                    AddToIncomingMessageQueue();
                                }
                            }
                            else {
                                std::cout << "[" << id << "] Read Header Fail." << std::endl;
                                m_socket.close();
                            }
                        });
                }

                void ReadBody() {
                    asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
                        [this](std::error_code ec, std::size_t length) {
                            if(!ec) {
                                AddToIncomingMessageQueue();
                            }
                            else {
                                std::cout << "[" << id << "] Read Body Fail: " << ec.message() << std::endl;
                                m_socket.close();
                            }
                        });
                }

                void WriteHeader() {
                    asio::async_write(m_socket, asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>)),
                        [this](std::error_code ec, std::size_t length) {
                            if(!ec) {
                                if(m_qMessagesOut.front().body.size() > 0) {
                                    WriteBody();
                                }
                                else {
                                    m_qMessagesOut.pop_front();

                                    if(!m_qMessagesOut.empty()) {
                                        WriteHeader();
                                    }
                                }
                            }
                            else {
                                std::cout << "[" << id << "] Write Header Fail.\n";
                                m_socket.close();
                            }
                        });
                }

                void WriteBody() {
                    asio::async_write(m_socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
                              [this](std::error_code ec, std::size_t length) {
                                  if(!ec) {
                                      m_qMessagesOut.pop_front();
                                      if (!m_qMessagesOut.empty())
                                      {
                                          WriteHeader();
                                      }
                                  }
                                  else
                                  {
                                      std::cout << "[" << id << "] Write Body Fail.\n";
                                      m_socket.close();
                                  }
                              });

                }

                void AddToIncomingMessageQueue() {
                    if(m_nOwnerType == owner::server)
                        m_qMessagesIn.push_back({ this->shared_from_this(), m_msgTemporaryIn });
                    else
                        m_qMessagesIn.push_back({ nullptr, m_msgTemporaryIn });

                    ReadHeader();
                }

                uint64_t scramble(uint64_t nInput) {
                    uint64_t out = nInput ^ 0xDEADBEEFC0DECAFE;
                    out = (out & 0xF0F0F0F0F0F0F0) >> 4 | (out & 0xF0F0F0F0F0F0F0) << 4;
                    return out ^ 0xC0DEFACE12345678;
                }

                void WriteValidation()
                {
                    asio::async_write(m_socket, asio::buffer(&m_nHandshakeOut, sizeof(uint64_t)),
                          [this](std::error_code ec, std::size_t length)
                          {
                                if(!ec) {
                                    if(m_nOwnerType == owner::client) {
                                        ReadHeader();
                                    }
                                }
                                else {
                                    m_socket.close();
                                }
                          });
                }

                void ReadValidation(olc::net::server_interface<T>* server = nullptr) {
                    asio::async_read(m_socket, asio::buffer(&m_nHandshakeIn, sizeof(uint64_t)),
                         [this, server](std::error_code ec, std::size_t length) {
                            if(!ec) {
                                if(m_nOwnerType == owner::server) {
                                    if(m_nHandshakeIn == m_nHandshakeCheck) {
                                        std::cout << "Client Validated" << std::endl;
                                        server->OnClientValidated(this->shared_from_this());

                                        ReadHeader();
                                    }
                                    else {
                                        std::cout << "Client Disconnected (Failed Validation)" << std::endl;
                                        m_socket.close();
                                    }
                                }
                                else {
                                    m_nHandshakeOut = scramble(m_nHandshakeIn);

                                    WriteValidation();
                                }
                            }
                            else {
                                std::cout << "Client Disconnected (Failed Validation): " << ec.message() << std::endl;
                                m_socket.close();
                            }
                         });
                }

            protected:
                //each connection has a unique socket to a remote
                asio::ip::tcp::socket m_socket;

                //This context is shared with the whole asio instance
                asio::io_context& m_asioContext;

                //This queue holds all messages to be sent to the remote side of this connection
                tsqueue<message<T>> m_qMessagesOut;

                //This queue holds all messages that have been received from the remote side of this connection;
                tsqueue<owned_message<T>>& m_qMessagesIn;
                message<T> m_msgTemporaryIn;

                owner m_nOwnerType = owner::server;
                uint32_t id = 0;

                uint64_t m_nHandshakeOut = 0;
                uint64_t m_nHandshakeIn = 0;
                uint64_t m_nHandshakeCheck = 0;
        };
    }
}