
#pragma once
#include <string>
#include "Database.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <cstdio>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/buffer.hpp>

namespace ba = boost::asio;
using SocketPtr = std::shared_ptr<ba::ip::tcp::socket>;

enum class ConnectionState
{
    UNREGISTERED,
    AUTHORIZED,
    CONNECTEDTOUSER,
    CONNECTEDTOCHAT
};

class UserState
{
    SocketPtr mSocketPtr;
    Database & mDb;
    ConnectionState connState = ConnectionState::UNREGISTERED;
    unsigned int userid;
    unsigned int currientChatID;
    unsigned int last_id = 0;
    std::atomic_bool mExit{false};
    
    std::unique_ptr<std::thread> mRecvThread;
    bool check_connection()
    {
        if(connState < ConnectionState::AUTHORIZED)
            return false;
        return true;
    }

    void recver_client_thread();
    void recver_client_chat_thread();

public:
    UserState(SocketPtr ptr, Database & db) : mDb(db), mSocketPtr(ptr) {}

    UserState(UserState&& state) : mSocketPtr(state.mSocketPtr), mDb(state.mDb), connState(state.connState), userid(state.userid), currientChatID(state.currientChatID), mRecvThread(std::move(state.mRecvThread)) {}

    UserState(UserState &) = delete;

    std::string register_new(const std::string & login, const std::string & password);

    std::string authorize(const std::string & login, const std::string & password);

    std::string send_message(const std::string & message);
    
    std::string getUserList();

    std::string connectToUser(const std::string & userName);
    std::string connectToChat(std::string & chat_name);
    
    std::string createChat(std::string chatName);

    std::string addUserToChat(std::string chatName, std::string userName);

    std::string getChats();

    std::string getMyChats();

    ~UserState();
};