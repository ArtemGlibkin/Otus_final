#include <iostream>
#include <vector>
#include <string>
#include "Database.h"

#include "server.h"

int main(int argc, char * argv[])
{

    Database db;

    Server server(db, 12345);
    db.create_message_table("messages");
    db.create_user_table("users");
    db.create_chat_tables();
    //db.register_user("artem", "123456");
    server.start();



    return 0;
}    