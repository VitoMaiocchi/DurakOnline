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
    
    std::map<unsigned int, unsigned int> opponent_cards; //Map ClientID to card count 
    unsigned int draw_pile_cards; 
    unsigned int trump_card; //the one that is on the bottom of the pile, can also be NULL
    unsigned int trump_suit;
    std::map<unsigned int, unsigned int> middle_cards; //map of slot to card
    std::vector<unsigned int> hand; //list of cards in hand
};

// provides an update on the players in the game
struct PlayerUpdate : public Message {
    PlayerUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::map<unsigned int, std::string> player_names;
    unsigned int number_players; 
    unsigned int durak; // the playerid of the loser/durak
};

// contains the meta battle state info
struct BattleStateUpdate : public Message {
    BattleStateUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    unsigned int defender; //there is only one defender
    std::vector<unsigned int> attackers; //list of attackers
    std::vector<unsigned int> idle; //list of observers/spectators
};


// tells the client what actions are available, in order to render the appropriate buttons
struct AvailableActionUpdate : public Message{
    AvailableActionUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    bool pass_on;
    bool ok;
    bool pick_up;
};


// update about the meta game state
// lobby, game, spectator, game over, Durak screen
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/* every state needs to be given a specific number so that everyone uses the same numbers*/
struct GameStateUpdate : public Message {
    GameStateUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    unsigned int state; 
};

//informs server that a player is trying to play a card -> specific slot?
struct PlayCardEvent : public Message {
    PlayCardEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::vector<Card> card; // can be multiple if multiple cards are played at once, max 4
    unsigned int slot; //place of the card
};

//info about which client action was performed
// cardclick, battlefieldclick, buttonclickDone, buttonclickPassOn, buttonclickPickUp
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/* every action needs to be given a specific number so that everyone uses the same numbers*/
struct ClientActionEvent : public Message {
    ClientActionEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    unsigned int action;
};

//info about the player
struct ClientConnectEvent : public Message {
    ClientConnectEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::string username;
};