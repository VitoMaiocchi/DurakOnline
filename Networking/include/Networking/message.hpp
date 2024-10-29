#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

enum MessageType { //nur so vorschlag mir chönds no us chäse han jetz eif gmacht wi uf dem white board
    MESSAGETYPE_TEST, //NUR FÜR TEST GRÜND DI CHÖMMER NACHER WEG NEH
    MESSAGETYPE_ILLEGAL_MOVE_NOTIFY,
    MESSAGETYPE_CARD_UPDATE,
    MESSAGETYPE_PLAYER_UPDATE,
    MESSAGETYPE_BATTLE_STATE_UPDATE,
    MESSAGETYPE_AVAILABLE_ACTION_UPDATE,
    MESSAGETYPE_GAME_STATE_UPDATE,
    MESSAGETYPE_PLAYCARD_EVENT,
    MESSAGETYPE_CLIENT_ACTION_EVENT,
    MESSAGETYPE_CLIENT_CONNECT_EVENT,
    MESSAGETYPE_CLIENT_DISCONNECT_EVENT
};

#define Allocator rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>

struct Message {
    MessageType messageType;
    std::string toJson() const;
    virtual void fromJson(const rapidjson::Value& obj) = 0;

    protected:
    virtual void getContent(rapidjson::Value &content, Allocator &allocator) const = 0;
};

#define MessagePtr std::unique_ptr<Message>;

std::unique_ptr<Message> deserialiseMessage(std::string string);

struct TestMessage : public Message {
    TestMessage();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    int x;
    int y;
    std::string string;
};

struct ClientDisconnectEvent : public Message {
    ClientDisconnectEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);
};

// send an error message to the player that the move was illegal
struct IllegalMoveNotify : public Message {
    IllegalMoveNotify();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::string error;
};

// tcard updates to the server and from the server to the client
struct CardUpdate : public Message{
    CardUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);
    
    std::map<unsigned int, unsigned int> opponentCards; //Map ClientID to card count 
    unsigned int drawPileCards; 
    unsigned int trumpCard; //the one that is on the bottom of the pile, can also be NULL
    unsigned int trumpSuit;
    std::map<unsigned int, unsigned int> middleCards; //map of slot to card
    std::vector<unsigned int> hand; //list of cards in hand
};


struct PlayerUpdate : public Message {
    PlayerUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);
    std::map<unsigned int, std::string> player_names;
    unsigned int number_players;
    unsigned int durak;
};