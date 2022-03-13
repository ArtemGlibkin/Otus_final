#include "UserState.h"

void UserState::recver_client_thread()
{
    while (!mExit)
    {
        auto messages = mDb.getMessages(currientChatID, userid, last_id);
        if (messages.second != -1)
        {
            mSocketPtr->write_some(ba::buffer(messages.first.c_str(), messages.first.size()));
            last_id = messages.second;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void UserState::recver_client_chat_thread()
{
    while (!mExit)
    {
        auto messages = mDb.getMessages(currientChatID, userid, last_id, true);
        if (messages.second != -1)
        {
            mSocketPtr->write_some(ba::buffer(messages.first.c_str(), messages.first.size()));
            last_id = messages.second;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::string UserState::register_new(const std::string &login, const std::string &password)
{
    int id = mDb.register_user(login, password);
    if (id != -1)
    {
        connState = ConnectionState::AUTHORIZED;
        userid = id;
        mDb.update_connected(userid, true);
        return "register ok";
    }
    return "register failed. Please retry";
}

std::string UserState::authorize(const std::string &login, const std::string &password)
{
    int id = mDb.check_user(login, password);
    if (id != -1)
    {
        connState = ConnectionState::AUTHORIZED;
        userid = id;
        mDb.update_connected(userid, true);
        return "authorize ok";
    }
    return "authorize failed";
}

std::string UserState::send_message(const std::string &message)
{
    if (!check_connection())
        return std::string("ERROR: You are not authorized or registered");
    if (currientChatID == 0)
        return std::string("ERROR: You are not connected to chat or user");
    if (!mDb.send_message(userid, currientChatID, message, (connState == ConnectionState::CONNECTEDTOCHAT ? true : false)))
        return "Sending error";
    return std::string("");
}

std::string UserState::getUserList()
{
    if (!check_connection())
        return std::string("ERROR: You are not authorized or registered");
    return "User list: \n" + mDb.getUserList();
}

std::string UserState::connectToUser(const std::string & userName)
{
    if (connState >= ConnectionState::CONNECTEDTOUSER)
        return std::string("ERROR: You are already connecte to chat or user");
    if (!check_connection())
        return std::string("ERROR: You are not authorized or registered");
    auto res = mDb.userIsValid(userName);
    if (res == -1)
        return std::string("ERROR: Chatid is not correct");
    connState = ConnectionState::CONNECTEDTOUSER;
    currientChatID = res;
    auto lastMessages = mDb.getLastUserMessages(userid, currientChatID);
    if (lastMessages.second != -1)
    {
        mSocketPtr->write_some(ba::buffer(lastMessages.first, lastMessages.first.size()));
        last_id = lastMessages.second;
    }
    mRecvThread = std::make_unique<std::thread>(&UserState::recver_client_thread, this);
    return std::string("Connected");
}

std::string UserState::connectToChat(std::string &chat_name)
{
    if (connState >= ConnectionState::CONNECTEDTOUSER)
        return std::string("ERROR: You are already connected to chat or user");
    if (!check_connection())
        return std::string("ERROR: You are not authorized or registered");

    int chatID = mDb.chatIsValid(chat_name);
    if (chatID == -1)
        return std::string("ERROR: Chatid is not correct");

    if (!mDb.isUserInChat(chat_name, userid))
        return std::string("ERROR: You do not have access to the chat");

    connState = ConnectionState::CONNECTEDTOCHAT;
    currientChatID = chatID;
    auto lastMessages = mDb.getLastChatMessages(userid, currientChatID);
    if (lastMessages.second != -1)
    {
        mSocketPtr->write_some(ba::buffer(lastMessages.first, lastMessages.first.size()));
        last_id = lastMessages.second;
    }
    mRecvThread = std::make_unique<std::thread>(&UserState::recver_client_chat_thread, this);
    return std::string("Connected");
}

std::string UserState::createChat(std::string chatName)
{
    if (!check_connection())
        return std::string("ERROR: You are not authorized or registered");

    if (!mDb.createChat(chatName, userid))
        return std::string("Error: chat was not created");
    return std::string("OK");
}

std::string UserState::addUserToChat(std::string chatName, std::string userName)
{
    if (!check_connection())
        return std::string("ERROR: You are not authorized or registered");
    if (!mDb.addUserToChat(chatName, userName))
        return "Error:: cannot add user to chat";
    return "OK";
}

std::string UserState::getChats()
{
    std::string chats = mDb.getChats(userid);
    if (chats.empty())
        return "You don`t have a chats";

    return std::string("Chats:\n") + chats;
}

std::string UserState::getMyChats()
{
    std::string chats = mDb.getOwnChats(userid);
    if (chats.empty())
        return "You don`t have a chats";

    return std::string("Chats:\n") + chats;
}

UserState::~UserState()
{
    mExit.store(true);
    if (mRecvThread)
        mRecvThread->join();
    if(connState >= ConnectionState::AUTHORIZED)
        mDb.update_connected(userid, false);
}