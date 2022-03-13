#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <ctime>
#include <pqxx/pqxx>

class Database
{
    std::unique_ptr<pqxx::connection> mHandler;

public:
    Database()
    {
        mHandler = std::make_unique<pqxx::connection>("postgresql://localhost");
    }

    bool create_message_table(const std::string &message_table);

    bool create_user_table(const std::string &user_table);
    bool create_chat_tables();
    int register_user(const std::string &user_name, const std::string &password);

    bool update_connected(int id, bool connected);

    int check_user(const std::string &user_name, const std::string &password);

    bool send_message(int srcid, int dstid, const std::string &message, bool chat);

    std::string getUserList();

    int userIsValid(const std::string & user);

    int chatIsValid(const std::string &chatName);

    bool createChat(const std::string chat_name, int ownerID);

    std::string getOwnChats(int ownerID);

    std::string getChats(int ownerID);

    bool isUserInChat(const std::string &chat_name, int userid);

    bool addUserToChat(std::string chat_name, std::string user_name);

    std::pair<std::string, int> getMessages(int srcid, int destid, int lastmessageid, bool chat = false);

    std::pair<std::string, int> getLastUserMessages(int srcid, int destid);

    std::pair<std::string, int> getLastChatMessages(int srcid, int destid);
};
