#pragma once
#include <map>
#include <string>
#include "Commands.h"

using CommandPtr = std::unique_ptr<Command>;
class CommandCreator
{
    std::map<std::string, std::function<CommandPtr(std::string &)>> mCommandCreators = {
        {"\\register", std::bind(&CommandCreator::registerCommand, this, std::placeholders::_1)},
        {"\\authorize", std::bind(&CommandCreator::authorizeCommand, this, std::placeholders::_1)},
        {"\\connect", std::bind(&CommandCreator::connectCommand, this, std::placeholders::_1)},
        {"\\connecttochat", std::bind(&CommandCreator::connectToChatCommand, this, std::placeholders::_1)},
        {"\\userlist", std::bind(&CommandCreator::userListCommand, this, std::placeholders::_1)},
        {"\\showchats", std::bind(&CommandCreator::getChatsCommand, this, std::placeholders::_1)},
        {"\\showmychats", std::bind(&CommandCreator::getMyChatsCommand, this, std::placeholders::_1)},
        {"\\addusertochat", std::bind(&CommandCreator::addUserToChatCommand, this, std::placeholders::_1)},
        {"\\createchat", std::bind(&CommandCreator::createChatCommand, this, std::placeholders::_1)},
        {"\\help", std::bind(&CommandCreator::createHelpCommand, this, std::placeholders::_1)}
    };
public:
    CommandPtr create(std::string &command, std::string &parameters)
    {
        if (command[0] != '\\')
            return std::make_unique<Message>(command);

        auto it = mCommandCreators.find(command);
        if (it == mCommandCreators.end())
        {
            std::cout << command << "   " << parameters << std::endl;
            return std::make_unique<UnknownCommand>();
        }
        return it->second(parameters);
    }

private:
    CommandPtr registerCommand(std::string &string)
    {
        std::string::size_type pos = string.find(" ");
        if (pos == std::string::npos)
            return std::make_unique<ErrorCommand>(std::string("ERROR:\\register [login] [password]"));
        std::string login = string.substr(0, pos);
        std::string password = string.substr(pos + 1);
        return std::make_unique<RegisterCommand>(login, password);
    }

    CommandPtr authorizeCommand(std::string &string)
    {
        std::string::size_type pos = string.find(" ");
        if (pos == std::string::npos)
            return std::make_unique<ErrorCommand>(std::string("ERROR:\\register [login] [password]"));
        std::string login = string.substr(0, pos);
        std::string password = string.substr(pos + 1);
        return std::make_unique<AuthorizeCommand>(login, password);
    }

    CommandPtr connectCommand(std::string &string)
    {
        return std::make_unique<ConnectCommand>(string);
    }

    CommandPtr connectToChatCommand(std::string &string)
    {
        return std::make_unique<ConnectToChatCommand>(string);
    }

    CommandPtr userListCommand(std::string &string)
    {
        return std::make_unique<UserListCommand>();
    }

    CommandPtr createChatCommand(std::string &string)
    {
        return std::make_unique<CreateChatCommand>(string);
    }

    CommandPtr getChatsCommand(std::string &string)
    {
        return std::make_unique<GetChatCommand>();
    }

    CommandPtr getMyChatsCommand(std::string &string)
    {
        return std::make_unique<GetMyChatsCommand>();
    }

    CommandPtr addUserToChatCommand(std::string &string)
    {
        std::string::size_type pos = string.find(" ");
        if (pos == std::string::npos)
            return std::make_unique<ErrorCommand>(std::string("ERROR:\\addUserToChat [chat] [user]"));
        std::string chat = string.substr(0, pos);
        std::string user = string.substr(pos + 1);
        return std::make_unique<AddUserToChatCommand>(chat, user);
    }

    CommandPtr createHelpCommand(std::string &string)
    {
        return std::make_unique<HelpCommand>();
    }
};

class CommandParser
{
    CommandCreator mCreator;

public:
    CommandPtr parse(std::string &string)
    {
        if (string[0] != '\\')
            return std::make_unique<Message>(string);

        std::string::size_type pos = string.find(" ");
        std::string command = string.substr(0, pos);
        std::string parameters;
        if (pos != std::string::npos)
            parameters = string.substr(pos + 1);
        return mCreator.create(command, parameters);
    }
};
