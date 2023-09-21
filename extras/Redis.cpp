//
//  main.cpp
//  RFID-Admin
//
//  Created by Ahmed Shehab on 9/17/23.
//

#include <iostream>
#include "/usr/local/include/hiredis/hiredis.h"

void connect_redis();
void get_redis_reply(redisContext *c,  redisReply *reply);
int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    connect_redis();
    return 0;
}
void connect_redis()
{
    char* host_ip="redis-13870.c56.east-us.azure.cloud.redislabs.com";
    int port = 13870;

    redisContext *c = redisConnect(host_ip, port);
    if (c != NULL && c->err) {
        std::cout<<"Error: "<< c->errstr<<std::endl;
        // handle error
    } else {

        std::cout<<"Connected to Redisn"<<std::endl;
    }

   redisReply *reply={};
    redisAppendCommand(c, "AUTH yNL4csFtTPnBN68hRAlzJFZpxWm2ScnN");
    get_redis_reply(c, reply);
    
    redisAppendCommand(c, "SET key %s", "hello c++");
    get_redis_reply(c, reply);
    
    redisAppendCommand(c, "GET key ");
    get_redis_reply(c, reply);
    
    redisAppendCommand(c, "keys *");
    get_redis_reply(c, reply);
    
    freeReplyObject(reply);
    redisFree(c);
}
void get_redis_reply(redisContext *c,  redisReply *reply)
{
    redisGetReply(c,(void**)&reply);
    if (reply->type == REDIS_REPLY_ARRAY) {
        for (int j = 0; j < reply->elements; j++) {
            printf("%u) %s\n", j, reply->element[j]->str);
        }
    }else{
        std::cout<<"Replied => "<< reply->str <<std::endl;
        
    }
    
}