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
    MESSAGETYPE_MOVEOPTION_UPDATE,
    MESSAGETYPE_GAME_STATE_UPDATE,
    MESSAGETYPE_PLAYCARD_EVENT,
    MESSAGETYPE_PRESS_BUTTON_EVENT,
    MESSAGETYPE_JOIN_EVEN
};

struct Message {
    MessageType messageType;
    virtual std::string toJson() const = 0;
    virtual void fromJson(const rapidjson::Value& obj) = 0;
};

std::unique_ptr<Message> deserialiseMessage(std::string string);

struct TestMessage : public Message {
    int x;
    int y;
    std::string string;

    TestMessage();
    std::string toJson() const;
    void fromJson(const rapidjson::Value& obj);
};

