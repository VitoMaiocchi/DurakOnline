#pragma once

#include <memory>
#include <string>
#include <vector>
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

