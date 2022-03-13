
#include "Database.h"

bool Database::create_message_table(const std::string &message_table)
{
    char *errMsg = nullptr;
    std::string query = "CREATE TABLE IF NOT EXISTS " + message_table + "(id serial PRIMARY KEY, srcid int, dstid int, dstchatid int, time timestamp NOT NULL, message text);";
    pqxx::work w(*mHandler);
    w.exec(query);
    w.commit();
    return true;
}

bool Database::create_user_table(const std::string &user_table)
{
    std::string query = "CREATE TABLE IF NOT EXISTS " + user_table + "(id serial PRIMARY KEY, user_name varchar(40), password varchar(40), is_connected bool DEFAULT false);";
    pqxx::work w(*mHandler);
    w.exec(query);
    w.commit();
    return true;
}

bool Database::create_chat_tables()
{
    std::string query = "CREATE TABLE IF NOT EXISTS chats(id serial PRIMARY KEY, chat_name varchar(40), ownerid int);";
    std::string query2 = "CREATE TABLE IF NOT EXISTS chats_participants(chatid int, userid int);";
    pqxx::work w(*mHandler);
    w.exec(query);
    w.exec(query2);
    w.commit();
    return true;
}

int Database::register_user(const std::string &user_name, const std::string &password)
{
    int res = -1;
    try
    {
        std::string query = "INSERT INTO users (user_name, password, is_connected) VALUES('" + user_name + "', '" + password + "'," + "true) RETURNING id;";
        pqxx::work w(*mHandler);
        auto result = w.exec(query);
        res = std::atoi(result.at(0)["id"].c_str());
        w.commit();
    }
    catch (const std::exception & ex)
    {
        std::cout<<ex.what()<<std::endl;
        return res;
    }
    return res;
}

bool Database::update_connected(int id, bool connected)
{
    std::string query = "UPDATE users SET is_connected = " + (connected ? std::string("true") : std::string("false")) + " WHERE id = " + std::to_string(id);
    try
    {
        pqxx::work w(*mHandler);
        w.exec(query);
        w.commit();
    }
    catch (const std::exception & ex)
    {
        std::cout<<ex.what()<<std::endl;
        return false;
    }
    return true;
}

int Database::check_user(const std::string &user_name, const std::string &password)
{
    std::string query = "select id from users where user_name='" + user_name + "' AND password = '" + password + "';";
    pqxx::work w(*mHandler);
    auto result = w.exec(query);
    if (result.size() == 0)
        return -1;
    return std::atoi(result.at(0)["id"].c_str());
}

bool Database::send_message(int srcid, int dstid, const std::string &message, bool chat)
{
    time_t now = time(0);

    std::string query = "insert into messages (srcid, " + (chat ? std::string("dstchatid") : std::string("dstid")) + ",  time, message) VALUES(" + std::to_string(srcid) + ", " + std::to_string(dstid) + ", '" + ctime(&now) + "', '" + message.c_str() + "');";
    pqxx::work w(*mHandler);
    try
    {
        w.exec(query);
        w.commit();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }

    return true;
}

std::string Database::getUserList()
{
    std::string query = "select id, user_name from users where is_connected = true";
    pqxx::work w(*mHandler);
    try
    {
        auto res = w.exec(query);
        if (res.size() == 0)
            return std::string("Empty list");

        std::stringstream ss;
        int id = 1;
        for(auto it : res)
        {
            ss<<id<<")"<<it["user_name"]<<std::endl;
            id++;
        }
        return ss.str();
    }
    catch(const std::exception & ex)
    {
        std::cout<<ex.what()<<std::endl;
    }
    return std::string();
}

int Database::userIsValid(const std::string & userName)
{
    std::string query = "select id from users where user_name = '" + userName + "'";
    pqxx::work w(*mHandler);
    auto res = w.exec(query);
    if (res.size() == 0)
        return -1;
    
    return std::atoi(res.at(0)["id"].c_str());
}

int Database::chatIsValid(const std::string &chatName)
{
    std::string query = "select id from chats where chat_name = '" + chatName + "'";
    pqxx::work w(*mHandler);
    auto res = w.exec(query);
    if (res.size() == 0)
        return -1;
    return std::atoi(res.at(0)["id"].c_str());
}

bool Database::createChat(const std::string chat_name, int ownerID)
{
    std::string query = "insert into chats (chat_name, ownerid) VALUES('" + chat_name + "', " + std::to_string(ownerID) + ");";
    pqxx::work w(*mHandler);
    try
    {
        auto res = w.exec(query);
        w.commit();
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        return false;
    }
    return true;
}

std::string Database::getOwnChats(int ownerID)
{
    std::string query = "select * from chats where ownerid = " + std::to_string(ownerID);
    std::stringstream ss;
    pqxx::work w(*mHandler);
    int counter = 1;
    try
    {
        auto res = w.exec(query);
        for (auto it : res)
        {
            ss << std::to_string(counter) + ") " << it["chat_name"] << std::endl;
            counter++;
        }
    }
    catch (...)
    {
    }
    return ss.str();
    //select * from chats inner join chats_participants on chats.id = chats_participants.chatid where userid = 1 or ownerid = 1;
}

std::string Database::getChats(int ownerID)
{
    auto id = std::to_string(ownerID);
    std::string query = "select * from chats left join chats_participants on chats.id = chats_participants.chatid where userid = " + id + " or ownerid = " + id;
    std::stringstream ss;
    pqxx::work w(*mHandler);
    int counter = 1;
    try
    {
        auto res = w.exec(query);
        for (auto it : res)
        {
            ss << std::to_string(counter) + ") " << it["chat_name"] << std::endl;
            counter++;
        }
    }
    catch (...)
    {
    }
    return ss.str();
}

bool Database::isUserInChat(const std::string &chat_name, int userid)
{
    auto id = std::to_string(userid);
    std::string query = " select 1 from chats left join chats_participants on chats.id = chats_participants.chatid where userid = " + id +
                        " or ownerid = " + id + " and chat_name = '" + chat_name + "'";
    pqxx::work w(*mHandler);
    int counter = 1;
    try
    {
        auto res = w.exec(query);
        if (res.size() == 0)
            return false;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool Database::addUserToChat(std::string chat_name, std::string user_name)
{
    std::string selectquery = "SELECT id from chats where chat_name = '" + chat_name + "'";
    std::string selectquery2 = "SELECT id from users where user_name = '" + user_name + "'";
    try
    {
        pqxx::work w(*mHandler);
        auto res = w.exec(selectquery);
        if (res.size() == 0)
            return false;
        int chatID = std::atoi(res.at(0)["id"].c_str());
        res = w.exec(selectquery2);
        if (res.size() == 0)
            return false;
        int userID = std::atoi(res.at(0)["id"].c_str());
        std::string query = "INSERT INTO chats_participants VALUES(" + std::to_string(chatID) + " ," + std::to_string(userID) + ")";
        res = w.exec(query);
        w.commit();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}

std::pair<std::string, int> Database::getMessages(int srcid, int destid, int lastmessageid, bool chat)
{
    std::string query = "select user_name, time, message, messages.id from messages inner join users on users.id = messages.srcid where messages.id > " + std::to_string(lastmessageid);
    if (!chat)
        query += " and srcid = " + std::to_string(srcid) + " and dstid = " + std::to_string(destid);
    else
        query += " and srcid <> " + std::to_string(destid) + " and dstchatid = " + std::to_string(srcid);
    pqxx::work w(*mHandler);
    auto res = w.exec(query);
    w.commit();
    if (res.size() == 0)
        return std::make_pair(std::string(), -1);
    std::stringstream ss;
    int last_id = -1;
    for (auto it : res)
    {
        ss << it["user_name"] << "[" << it["time"] << "]: " << it["message"] << std::endl;
        last_id = std::atoi(it["id"].c_str());
    }
    return std::make_pair(ss.str(), last_id);
}

std::pair<std::string, int> Database::getLastUserMessages(int srcid, int destid)
{
    std::string query = "select user_name, time, message, messages.id from messages inner join users on users.id = messages.srcid where (srcid = " + std::to_string(srcid) +
                        " and dstid = " + std::to_string(destid) + ") or (srcid = " + std::to_string(destid) + " and dstid = " + std::to_string(srcid) + ");";
    pqxx::work w(*mHandler);
    auto res = w.exec(query);
    w.commit();
    if (res.size() == 0)
        return std::make_pair(std::string(), -1);
    std::stringstream ss;
    int last_id = -1;
    for (auto it : res)
    {
        ss << it["user_name"] << "[" << it["time"] << "]: " << it["message"] << std::endl;
        last_id = std::atoi(it["id"].c_str());
    }
    return std::make_pair(ss.str(), last_id);
}

std::pair<std::string, int> Database::getLastChatMessages(int srcid, int destid)
{
    std::string query = "select user_name, time, message, messages.id from messages inner join users on users.id = messages.srcid where dstchatid = " + std::to_string(destid);
    pqxx::work w(*mHandler);
    auto res = w.exec(query);
    w.commit();
    if (res.size() == 0)
        return std::make_pair(std::string(), -1);
    std::stringstream ss;
    int last_id = -1;
    for (auto it : res)
    {
        ss << it["user_name"] << "[" << it["time"] << "]: " << it["message"] << std::endl;
        last_id = std::atoi(it["id"].c_str());
    }
    return std::make_pair(ss.str(), last_id);
}