#pragma once
#include <string>
#include "UserState.h"

class Command
{
public:
    Command()
    {
    }
    virtual std::string execute(UserState &state) = 0;
};

class Message : public Command
{
    std::string mMessage;

public:
    Message(const std::string &message) : mMessage(message) {}
    std::string execute(UserState &state)
    {
        return state.send_message(mMessage);
        //return db.insert(mTableName, mID, mValue);
    }
};

class RegisterCommand : public Command
{
    std::string mLogin;
    std::string mPassword;

public:
    RegisterCommand(const std::string &login, const std::string &password) : mLogin(login), mPassword(password) {}
    std::string execute(UserState &state)
    {
        return state.register_new(mLogin, mPassword);
    }
};

class AuthorizeCommand : public Command
{
    std::string mLogin;
    std::string mPassword;

public:
    AuthorizeCommand(const std::string &login, const std::string &password) : mLogin(login), mPassword(password) {}

    std::string execute(UserState &state)
    {
        return state.authorize(mLogin, mPassword);
    }
};

class ConnectCommand : public Command
{
    std::string mChatName;
public:
    ConnectCommand(const std::string &chat) : mChatName(chat){}
    std::string execute(UserState &state)
    {
        return state.connectToUser(mChatName);
    }
};

class ConnectToChatCommand : public Command
{
    std::string mChat;

public:
    ConnectToChatCommand(const std::string & command) : mChat(command)
    {
    }

    std::string execute(UserState &state)
    {
        return state.connectToChat(mChat);
    }
};

class CreateChatCommand : public Command
{
    std::string mChatName;

public:
    CreateChatCommand(const std::string & name) : mChatName(name) {}

    std::string execute(UserState &state)
    {
        return state.createChat(mChatName);
    }
};

class GetChatCommand : public Command
{
public:
    std::string execute(UserState &state)
    {
        return state.getChats();
    }
};

class GetMyChatsCommand : public Command
{
public:
    std::string execute(UserState &state)
    {
        return state.getMyChats();
    }
};

class AddUserToChatCommand : public Command
{
    std::string mChatName;
    std::string mUserName;
public:
    AddUserToChatCommand(const std::string & chatname, const std::string & username) : mChatName(chatname), mUserName(username){}

    std::string execute(UserState &state)
    {
        return state.addUserToChat(mChatName, mUserName);
    }
};

class UserListCommand : public Command
{
public:
    std::string execute(UserState &state)
    {
        return state.getUserList();
    }
};

class UnknownCommand : public Command
{
public:
    std::string execute(UserState &state)
    {
        return "ERROR: Unknown Command";
    }
};

class ErrorCommand : public Command
{
    std::string mError;
public:
    ErrorCommand(const std::string &command) : mError(command) {}
    std::string execute(UserState &state)
    { 
        return mError;
    }
};

class HelpCommand : public Command
{
public:
    std::string execute(UserState &state)
    { 
        std::stringstream ss;
        ss<<"Commands:"<<std::endl;
        ss<<"\\register [user] [password] - register new user"<<std::endl;
        ss<<"\\authorize [user] [password] - authorization"<<std::endl;
        ss<<"\\connect [username] - connect to user to user conversation"<<std::endl;
        ss<<"\\connecttochat [chatname] - connect to multiuser conversation"<<std::endl;
        ss<<"\\userlist - online userlist"<<std::endl;
        ss<<"\\showchats - show accesible chats"<<std::endl;
        ss<<"\\showmychats - show own chats"<<std::endl;
        ss<<"\\addusertochat [username] - add user to chat"<<std::endl;
        ss<<"\\createchat [chatname] - create new own chat"<<std::endl;
        ss<<"\\help - full information about commands"<<std::endl;
        return ss.str();
    }
};