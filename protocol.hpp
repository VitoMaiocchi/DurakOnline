#pragma once

/*
    DURAK ONLINE NETWORK PROTOCOL
    Version 2.0

    das nur ändere wenns abgsproche isch
    das isch s most upstream stück code
*/

typedef unsigned int uint;

#include <string>
#include <memory>
#include <map>
#include <list>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <iostream> //TODO: add logger

namespace Protocol {

enum Suit {
    SUIT_CLUBS,
    SUIT_SPADES,
    SUIT_DIAMONDS,
    SUIT_HEARTS,
    SUIT_count
};

enum Rank {
    RANK_TWO,
    RANK_THREE,
    RANK_FOUR,
    RANK_FIVE,
    RANK_SIX, 
    RANK_SEVEN,
    RANK_EIGHT,
    RANK_NINE,
    RANK_TEN,
    RANK_JACK,
    RANK_QUEEN,
    RANK_KING,
    RANK_ACE,
    RANK_count
};

enum CardSlot {
    CARDSLOT_1,
    CARDSLOT_2,
    CARDSLOT_3,
    CARDSLOT_4,
    CARDSLOT_5,
    CARDSLOT_6,
    CARDSLOT_1_TOP, //für karte wo uf de andere ligged
    CARDSLOT_2_TOP,
    CARDSLOT_3_TOP,
    CARDSLOT_4_TOP,
    CARDSLOT_5_TOP,
    CARDSLOT_6_TOP,
    CARDSLOT_NONE
};

enum GameAction {
    GAMEACTION_READY,
    GAMEACTION_PASS_ON,
    GAMEACTION_PICK_UP,
};

enum LobbyAction {
    LOBBYACTION_BACK_TO_LOBBY,
    LOBBYACTION_READY,
};

enum PlayerRole {
    ATTACKER,
    DEFENDER,
    CO_ATTACKER,
    IDLE,
    FINISHED
};

enum GameStage {
    // da so first attack, post pick up etc
    GAMESTAGE_FIRST_ATTACK,
    GAMESTAGE_DEFEND,
    GAMESTAGE_POST_PICKUP,
};

struct Card {
    Rank rank;
    Suit suit;

    inline Card(Rank rank, Suit suit) : rank(rank), suit(suit) {}
    inline Card(uint i)  {
    suit = static_cast<Suit>(i/RANK_count);
    rank = static_cast<Rank>(i%RANK_count);
    }

    inline const uint toInt() const {
        return ((uint) suit * (uint) RANK_count + (uint) rank);
    }

    inline bool operator==(const Card& other) const {
        return rank==other.rank && suit==other.suit;
    }

    inline bool operator!=(const Card& other) const {
        return rank!=other.rank || suit!=other.suit;
    }
};

typedef uint64_t PlayerUUID;

// MESSAGES


enum MessageType { 
    //Layer 1 (da burchts no paar)
    SERVERMESSAGE_GAMESELECTION,         //example. da chammer no me lobby stuff adde
    CLIENTMESSAGE_REQUEST_USER_DATA,     //user data über e uuid requeste
    SERVERMESSAGE_USER_DATA,             //returns user data (name, stats, bild, etc)

    //Layer 2 
    SERVERMESSAGE_LOBBY_UPDATE,          //lobby state (includes players and ready state)
    SERVERMESSAGE_GAMEOVER_UPDATE,       //game over state mit durak
    CLIENTMESSAGE_LOBBY_ACTION_EVENT,     //ready in lobby (obviously)

    //Layer 3
    SERVERMESSAGE_GAME_UPDATE_PUBLIC,    //publicly visible game screen (broadcasted to everyone including spectators)
    SERVERMESSAGE_GAME_UPDATE_PRIVATE,   //privatly visible game screen (cards and available actions)
    SERVERMESSAGE_TIMER_UPDATE,          //timer applies to lobby and game

    //Layer 4
    CLIENTMESSAGE_GAME_ACTION_EVENT,     //player action (ready, reflect, etc)
    CLIENTMESSAGE_PLAY_CARD_EVENT,       //player playing card
};

typedef rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> Allocator;

struct Message {
    virtual ~Message() = default;
    MessageType messageType;
    std::string toJson() const;
    virtual void fromJson(const rapidjson::Value& obj) = 0;

    protected:
    virtual void getContent(rapidjson::Value &content, Allocator &allocator) const = 0;
};

std::unique_ptr<Message> deserialiseMessage(std::string string);

//LAYER 1

struct ClientMessageRequestUserData : public Message {
    ClientMessageRequestUserData();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::list<PlayerUUID> players;
};

struct ServerMessageUserData : public Message {
    ServerMessageUserData();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    PlayerUUID player;
    std::string user_name;
    //da chammer denn no meh sache adde
};

//LAYER 2

struct ServerMessageLobbyUpdate : public Message {
    ServerMessageLobbyUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::list<PlayerUUID> players;
    std::list<PlayerUUID> ready;
};


struct ServerMessageGameoverUpdate : public Message {
    ServerMessageGameoverUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::list<PlayerUUID> players;
    PlayerUUID durak;
};

struct ClientMessageLobbyActionEvent : public Message {
    ClientMessageLobbyActionEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    LobbyAction action;
};

//LAYER 3

struct ServerMessageGameUpdatePublic : public Message {
    ServerMessageGameUpdatePublic();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    GameStage stage;
    uint draw_pile_cards;
    Card trump_card; //wenn draw pile cards 0 isch rendered eifach nur de suit
    std::map<CardSlot, Card> middle_cards;
    std::map<PlayerUUID, uint> player_card_count;
    std::map<PlayerUUID, PlayerRole> player_roles;
};

struct ServerMessageGameUpdatePrivate : public Message {
    ServerMessageGameUpdatePrivate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::list<GameAction> available_actions;
    std::list<Card> cards;
};

struct ServerMessageTimerUpdate : public Message {
    ServerMessageTimerUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    uint time_left; //in ms
};

//LAYER 4

struct ClientMessageGameActionEvent : public Message {
    ClientMessageGameActionEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    GameAction action;
};

struct ClientMessagePlayCardEvent : public Message {
    ClientMessagePlayCardEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    CardSlot slot;
    std::list<Card> cards;
};


#ifdef DURAK_PROTOCOL_IMPLEMENTATION

std::unique_ptr<Message> deserialiseMessage(std::string string) {

    rapidjson::Document document;
    // assert(!document.Parse(string.c_str()).HasParseError());
    if(document.Parse(string.c_str()).HasParseError()){
        std::cerr << "Error: JSON parsing failed." << std::endl;
        return nullptr;
    }

    if (!document.HasMember("message_type") || !document["message_type"].IsInt()) {
        std::cerr << "Error: 'message_type' is missing or not an integer." << std::endl;
        return nullptr;
    }
    MessageType type = static_cast<MessageType>(document["message_type"].GetInt());

    if (!document.HasMember("content") || !document["content"].IsObject()) {
        std::cerr << "Error: 'content' is missing or not an object." << std::endl;
        return nullptr;
    }

    rapidjson::Value &content = document["content"];

    std::unique_ptr<Message> message;
    switch (type) {
        case CLIENTMESSAGE_REQUEST_USER_DATA:
            message = std::make_unique<ClientMessageRequestUserData>();
        break;
        case SERVERMESSAGE_USER_DATA:
            message = std::make_unique<ServerMessageUserData>();
        break;
        case SERVERMESSAGE_LOBBY_UPDATE:
            message = std::make_unique<ServerMessageLobbyUpdate>();
        break;
        case SERVERMESSAGE_GAMEOVER_UPDATE:
            message = std::make_unique<ServerMessageGameoverUpdate>();
        break;
        case CLIENTMESSAGE_LOBBY_ACTION_EVENT:
            message = std::make_unique<ClientMessageLobbyActionEvent>();
        break;
        case SERVERMESSAGE_GAME_UPDATE_PUBLIC:
            message = std::make_unique<ServerMessageGameUpdatePublic>();
        break;
        case SERVERMESSAGE_GAME_UPDATE_PRIVATE:
            message = std::make_unique<ServerMessageGameUpdatePrivate>();
        break;
        case SERVERMESSAGE_TIMER_UPDATE:
            message = std::make_unique<ServerMessageTimerUpdate>();
        break;
        case CLIENTMESSAGE_GAME_ACTION_EVENT:
            message = std::make_unique<ClientMessageGameActionEvent>();
        break;
        case CLIENTMESSAGE_PLAY_CARD_EVENT:
            message = std::make_unique<ClientMessagePlayCardEvent>();
        break;
        default:
            std::cout << "ahhh irgend en messagetype fehlt no in message.cpp" << std::endl;
        break;
    }

    if(message){
        message->fromJson(content);
    } else{
        std::cerr << "Error: message creation failed" << std::endl;
    }

    return message;
}

std::string Message::toJson() const {
    rapidjson::Document document;
    document.SetObject();
    auto &allocator = document.GetAllocator();

    rapidjson::Value content(rapidjson::kObjectType);
    getContent(content, allocator);
    document.AddMember("message_type", static_cast<uint>(messageType), allocator);
    document.AddMember("content", content, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return buffer.GetString();
}


// LAYER 1

ClientMessageRequestUserData::ClientMessageRequestUserData() { messageType = CLIENTMESSAGE_REQUEST_USER_DATA; }
void ClientMessageRequestUserData::getContent(rapidjson::Value &content, Allocator &allocator) const {
    rapidjson::Value player_array(rapidjson::kArrayType);
    for (const auto& player : players) {
        player_array.PushBack(rapidjson::Value().SetUint64(player), allocator);
    }
    content.AddMember("players", player_array, allocator);
}
void ClientMessageRequestUserData::fromJson(const rapidjson::Value& obj) {
    for (const auto& player : obj["players"].GetArray()) {
        players.push_back(player.GetUint64());
    }
}

ServerMessageUserData::ServerMessageUserData() { messageType = SERVERMESSAGE_USER_DATA; }
void ServerMessageUserData::getContent(rapidjson::Value &content, Allocator &allocator) const { /*TODO*/ }
void ServerMessageUserData::fromJson(const rapidjson::Value& obj) { /*TODO*/ }

ServerMessageLobbyUpdate::ServerMessageLobbyUpdate() { messageType = SERVERMESSAGE_LOBBY_UPDATE; }
void ServerMessageLobbyUpdate::getContent(rapidjson::Value &content, Allocator &allocator) const { /*TODO*/ }
void ServerMessageLobbyUpdate::fromJson(const rapidjson::Value& obj) { /*TODO*/ }

ServerMessageGameoverUpdate::ServerMessageGameoverUpdate() { messageType = SERVERMESSAGE_GAMEOVER_UPDATE; }
void ServerMessageGameoverUpdate::getContent(rapidjson::Value &content, Allocator &allocator) const { /*TODO*/ }
void ServerMessageGameoverUpdate::fromJson(const rapidjson::Value& obj) { /*TODO*/ }

ClientMessageLobbyActionEvent::ClientMessageLobbyActionEvent() { messageType = CLIENTMESSAGE_LOBBY_ACTION_EVENT; }
void ClientMessageLobbyActionEvent::getContent(rapidjson::Value &content, Allocator &allocator) const { /*TODO*/ }
void ClientMessageLobbyActionEvent::fromJson(const rapidjson::Value& obj) { /*TODO*/ }

ServerMessageGameUpdatePublic::ServerMessageGameUpdatePublic() : trump_card(0) { messageType = SERVERMESSAGE_GAME_UPDATE_PUBLIC; }
void ServerMessageGameUpdatePublic::getContent(rapidjson::Value &content, Allocator &allocator) const { /*TODO*/ }
void ServerMessageGameUpdatePublic::fromJson(const rapidjson::Value& obj) { /*TODO*/ }

ServerMessageGameUpdatePrivate::ServerMessageGameUpdatePrivate() { messageType = SERVERMESSAGE_GAME_UPDATE_PRIVATE; }
void ServerMessageGameUpdatePrivate::getContent(rapidjson::Value &content, Allocator &allocator) const { /*TODO*/ }
void ServerMessageGameUpdatePrivate::fromJson(const rapidjson::Value& obj) { /*TODO*/ }

ServerMessageTimerUpdate::ServerMessageTimerUpdate() { messageType = SERVERMESSAGE_TIMER_UPDATE; }
void ServerMessageTimerUpdate::getContent(rapidjson::Value &content, Allocator &allocator) const { /*TODO*/ }
void ServerMessageTimerUpdate::fromJson(const rapidjson::Value& obj) { /*TODO*/ }

ClientMessageGameActionEvent::ClientMessageGameActionEvent() { messageType = CLIENTMESSAGE_GAME_ACTION_EVENT; }
void ClientMessageGameActionEvent::getContent(rapidjson::Value &content, Allocator &allocator) const { /*TODO*/ }
void ClientMessageGameActionEvent::fromJson(const rapidjson::Value& obj) { /*TODO*/ }

ClientMessagePlayCardEvent::ClientMessagePlayCardEvent() { messageType = CLIENTMESSAGE_PLAY_CARD_EVENT; }
void ClientMessagePlayCardEvent::getContent(rapidjson::Value &content, Allocator &allocator) const { /*TODO*/ }
void ClientMessagePlayCardEvent::fromJson(const rapidjson::Value& obj) { /*TODO*/ }

#endif
}