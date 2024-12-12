#pragma once

#include <string>
#include <memory>
#include <Networking/message.hpp>

typedef unsigned int uint;
typedef uint ClientID;

/*
HIGH LEVEL OVERVIEW OF NETWORKING:

(network_client.cpp, network_server.cpp)
the networking cpps are responsible for proving all network interactions
each client opens 2 connections. For receving and sending. 
The messages are recived on separate threads. Each recive thread adds 
any messages to a massge queue. The newest message in the message queue
can be fetched by calling recive message.
The server version of recive Message blocks until a new message arrives.
The client version returns null if nothing is recived to not block
rendering frames wich happens on the same thread.

(message.cpp)
All message that are sent are of the abstact class Message
The message class is responsible for serializing and deserializing any messages
The different Message types are derived from Message and have to implment
a to and from json function
more detail about the message types in message.hpp, message.cpp
*/

namespace Network {
    //SERVER USE
    #ifdef NETWORKTYPE_SERVER
    void openSocket(uint port);
    bool sendMessage(std::unique_ptr<Message> message, ClientID id); //returns false if client id not valid or disconnected
    std::unique_ptr<Message> reciveMessage(ClientID &id);
    #endif

    //CLIENT USE
    #ifdef NETWORKTYPE_CLIENT
    ClientID openConnection(std::string ip, uint port);
    void closeConnection();
    void sendMessage(std::unique_ptr<Message> message);
    std::unique_ptr<Message> reciveMessage();
    #endif
}