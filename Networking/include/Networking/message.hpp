#pragma once

#include <memory>
#include <string>
#include <map>
#include <unordered_set>
#include <list>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <Networking/util.hpp>

enum MessageType {
    MESSAGETYPE_TEST,
    // Server:* to Client:MasterNode
    MESSAGETYPE_ILLEGAL_MOVE_NOTIFY, // Server:Battle to Client:MasterNode, notifies the client that the move was illegal
    MESSAGETYPE_CARD_UPDATE, // Server:CardManager to Client:MasterNode, communicates the current status of the cards in play
    MESSAGETYPE_PLAYER_UPDATE, // Server:Game to Client:MasterNode, provides an update on the players in the game
    MESSAGETYPE_BATTLE_STATE_UPDATE, // Server:Battle to Client:MasterNode, contains info on player roles (attackers, defender, idle)
    MESSAGETYPE_AVAILABLE_ACTION_UPDATE, // Server:Battle to Client:MasterNode, tells the client what actions are available
    MESSAGETYPE_GAME_STATE_UPDATE, // Server:* to Client:MasterNode, update game screen (lobby, game, spectator, game over, Durak screen)
    // Client:MasterNode to Server:*
    MESSAGETYPE_PLAYCARD_EVENT, // Client:MasterNode to Server:Server->Game->Battle, informs server that a player is trying to play a card
    MESSAGETYPE_CLIENT_ACTION_EVENT, // Client:MasterNode to Server:Server->Game->Battle, info about which client action was performed
    MESSAGETYPE_CLIENT_CONNECT_EVENT, // Client:MasterNode to Server:Server->Game, info about the player
    MESSAGETYPE_REMOTE_DISCONNECT_EVENT // Client:MasterNode to Server:Server->Game, informs server that a player has disconnected
};

typedef rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> Allocator;

struct Message {
    MessageType messageType;
    std::string toJson() const;
    virtual void fromJson(const rapidjson::Value& obj) = 0;

    protected:
    virtual void getContent(rapidjson::Value &content, Allocator &allocator) const = 0;
};

typedef std::unique_ptr<Message> MessagePtr;

std::unique_ptr<Message> deserialiseMessage(std::string string);

struct TestMessage : public Message {
    TestMessage();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    int x;
    int y;
    std::string string;
};

struct RemoteDisconnectEvent : public Message {
    RemoteDisconnectEvent();
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
    
    std::map<ClientID, unsigned int> opponent_cards; //Map ClientID to card count 
    unsigned int draw_pile_cards; 
    Card trump_card; //the one that is on the bottom of the pile, can also be NULL
    Suit trump_suit;
    std::map<CardSlot, Card> middle_cards; //map of slot to card
    std::list<Card> hand; //list of cards in hand
};

// provides an update on the players in the game
struct PlayerUpdate : public Message {
    PlayerUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::map<ClientID, std::string> player_names;
    unsigned int player_count; 
    ClientID durak; // the playerid of the loser/durak
};

// contains the meta battle state info
struct BattleStateUpdate : public Message {
    BattleStateUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    ClientID defender; //there is only one defender
    std::list<ClientID> attackers; //list of attackers
    std::list<ClientID> idle; //list of observers/spectators
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

struct GameStateUpdate : public Message {
    GameStateUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    GameState state; 
};

//informs server that a player is trying to play a card -> specific slot?
struct PlayCardEvent : public Message {
    PlayCardEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::unordered_set<Card> cards; // can be multiple if multiple cards are played at once, max 4
    CardSlot slot; //place of the card
};

//info about which client action was performed
// buttonclickDone, buttonclickPassOn, buttonclickPickUp
// cardclick, battlefieldclick bruchts nöd das isch client side
struct ClientActionEvent : public Message {
    ClientActionEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    ClientAction action;
};

//info about the player
struct ClientConnectEvent : public Message {
    ClientConnectEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::string username;
};