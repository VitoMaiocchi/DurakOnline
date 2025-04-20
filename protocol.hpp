#pragma once

/*
    DURAK ONLINE NETWORK PROTOCOL
    Version 2.0

    das nur ändere wenns abgsproche isch
    das isch s most upstream stück code
*/

#define uint unsigned int

#include <string>
#include <memory>
#include <map>
#include <list>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

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
};

struct Card {
    Suit suit;
    Rank rank;

    inline Card(Rank rank, Suit suit) : rank(rank), suit(suit) {}
    inline Card(uint i)  {
    suit = static_cast<Suit>(i/RANK_count);
    rank = static_cast<Rank>(i%RANK_count);
    }

    inline const uint toInt() const {
        return (suit * RANK_count + rank);
    }

    inline bool operator==(const Card& other) const {
        return rank==other.rank && suit==other.suit;
    }

    inline bool operator!=(const Card& other) const {
        return rank!=other.rank || suit!=other.suit;
    }
};

#define PlayerUUID uint64_t

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
    SERVERMESSAGE_TIME_RUPDATE,          //timer applies to lobby and game

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



//das isch s alte protocol implementaiton. das muss mer jetz neu mache aber ich lans no für reference
//TODO: das neu mache

#ifdef DURAK_PROTOCOL_IMPLEMENTATION

#include <algorithm> // for std::all_of for debugging purposes
#include <iostream>

//std::unique_ptr<Message> deserialiseMessage(std::string string) {
    //std::cout << "Trying to deserialise Message: \n" << string << std::endl;
    
    /*
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
        case MESSAGETYPE_TEST:
            message = std::make_unique<TestMessage>();
        break;
        case MESSAGETYPE_REMOTE_DISCONNECT_EVENT:
            message = std::make_unique<RemoteDisconnectEvent>();
        break;
        case MESSAGETYPE_SEND_POPUP:
            message = std::make_unique<PopupNotify>();
        break;
        case MESSAGETYPE_CARD_UPDATE:
            message = std::make_unique<CardUpdate>();
        break;
        case MESSAGETYPE_PLAYER_UPDATE:
            message = std::make_unique<PlayerUpdate>();
        break;
        case MESSAGETYPE_BATTLE_STATE_UPDATE:
            message = std::make_unique<BattleStateUpdate>();
        break;
        case MESSAGETYPE_AVAILABLE_ACTION_UPDATE:
            message = std::make_unique<AvailableActionUpdate>();
        break;
        case MESSAGETYPE_GAME_STATE_UPDATE:
            message = std::make_unique<GameStateUpdate>();
        break;
        case MESSAGETYPE_PLAYCARD_EVENT:
            message = std::make_unique<PlayCardEvent>();
        break;
        case MESSAGETYPE_CLIENT_ACTION_EVENT:
            message = std::make_unique<ClientActionEvent>();
        break;
        case MESSAGETYPE_CLIENT_CONNECT_EVENT:
            message = std::make_unique<ClientConnectEvent>();
        break;
        case MESSAGETYPE_READY_UPDATE:
            message = std::make_unique<ReadyUpdate>();
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
    // std::cout << "yay, omg i did it"<< std::endl;
    return buffer.GetString();
}

//TEST MESSAGE
TestMessage::TestMessage() {messageType = MESSAGETYPE_TEST;}

void TestMessage::getContent(rapidjson::Value &content, Allocator &allocator) const {
    content.AddMember("x", x, allocator);
    content.AddMember("y", y, allocator);
    content.AddMember("string", rapidjson::Value(string.c_str(), allocator) , allocator);
};

void TestMessage::fromJson(const rapidjson::Value& obj) {
    x = obj["x"].GetInt();
    y = obj["y"].GetInt();
    string = obj["string"].GetString();
};


ReadyUpdate::ReadyUpdate() {messageType = MESSAGETYPE_READY_UPDATE;}

void ReadyUpdate::getContent(rapidjson::Value &content, Allocator &allocator) const {
    rapidjson::Value playersJson(rapidjson::kArrayType);
    for(const auto p : players){
        playersJson.PushBack(p, allocator);
    }
    content.AddMember("players", playersJson, allocator);
}

void ReadyUpdate::fromJson(const rapidjson::Value& obj) {
    players.clear();
    const rapidjson::Value& playersJson = obj["players"];
    for(rapidjson::SizeType i = 0; i < playersJson.Size(); ++i) players.insert(playersJson[i].GetUint());
}


//CLIENT DISCONNECT (dummy message; only sent my networking therefore the content is empty)
RemoteDisconnectEvent::RemoteDisconnectEvent() {messageType = MESSAGETYPE_REMOTE_DISCONNECT_EVENT;}
void RemoteDisconnectEvent::getContent(rapidjson::Value &content, Allocator &allocator) const {}
void RemoteDisconnectEvent::fromJson(const rapidjson::Value& obj) {}


//ILLEGAL MOVE NOTIFY
PopupNotify::PopupNotify(){messageType = MESSAGETYPE_SEND_POPUP;}

void PopupNotify::getContent(rapidjson::Value &content, Allocator &allocator) const{
    content.AddMember("error", rapidjson::Value(message.c_str(), allocator), allocator);
}

void PopupNotify::fromJson(const rapidjson::Value& obj){
    message = obj["error"].GetString();
}

// CARD UPDATE
CardUpdate::CardUpdate() : trump_card(Card(0)) {
    messageType = MESSAGETYPE_CARD_UPDATE;
    
}

void CardUpdate::getContent(rapidjson::Value &content, Allocator &allocator) const{
    
    //add opponent_cards which is a std::map
    // we iterate through the pairs and add each key-value pair
    //as a member of JSON
    rapidjson::Value opponentCardsJson(rapidjson::kObjectType);
    for(const auto &pair : opponent_cards){
        rapidjson::Value key(std::to_string(pair.first).c_str(), allocator);
        rapidjson::Value value(pair.second);
        opponentCardsJson.AddMember(key, value, allocator);
    }
    content.AddMember("opponent_cards", opponentCardsJson, allocator);

    content.AddMember("draw_pile_cards", draw_pile_cards, allocator);
    content.AddMember("trump_card", trump_card.toInt(), allocator);
    content.AddMember("trump_suit", trump_suit, allocator);

    rapidjson::Value middleCardJson(rapidjson::kObjectType);
    for(const auto &pair : middle_cards){
        middleCardJson.AddMember(
            rapidjson::Value(std::to_string(pair.first).c_str(), allocator), //slot
            rapidjson::Value(pair.second.toInt()).Move(), //card
            allocator
        );
    }
    content.AddMember("middle_cards", middleCardJson, allocator);

    //adds the vector hand to json 
    rapidjson::Value handJson(rapidjson::kArrayType);
    for(const auto card : hand){
        handJson.PushBack(card.toInt(), allocator);
    }
    content.AddMember("hand", handJson, allocator);
}
void CardUpdate::fromJson(const rapidjson::Value& obj){
    //turning opponent cards back to map
    const rapidjson::Value& opponentCardsJson = obj["opponent_cards"];
    for(auto itr = opponentCardsJson.MemberBegin(); itr != opponentCardsJson.MemberEnd(); ++itr){
        unsigned int cliend_id = std::stoi(itr->name.GetString());
        unsigned int card_count = itr->value.GetUint();
        opponent_cards[cliend_id] = card_count;
    }

    draw_pile_cards= obj["draw_pile_cards"].GetUint();
    trump_card = obj["trump_card"].GetUint();
    trump_suit = static_cast<Suit>(obj["trump_suit"].GetUint());

    //middlecards back to map
    const  rapidjson::Value& middleCardJson = obj["middle_cards"];
    for(auto itr = middleCardJson.MemberBegin(); itr != middleCardJson.MemberEnd(); ++itr){
        unsigned int slot = std::stoi(itr->name.GetString());
        unsigned int card = itr->value.GetUint();
        middle_cards[static_cast<CardSlot>(slot)] = card;
    }

    const rapidjson::Value& handJson = obj["hand"];
    hand.clear(); // so that we only store the latest data
    for(rapidjson::SizeType i = 0; i < handJson.Size(); ++i){
        hand.push_back(handJson[i].GetUint());
    }
}


//PLAYER UPDATE
PlayerUpdate::PlayerUpdate() {messageType = MESSAGETYPE_PLAYER_UPDATE;}

void PlayerUpdate::getContent(rapidjson::Value &content, Allocator &allocator) const {

    rapidjson::Value playerNamesJson(rapidjson::kObjectType);
    for(const auto& [key, value] : player_names){
        playerNamesJson.AddMember(
            rapidjson::Value(std::to_string(key).c_str(), allocator), //the player id to a string
            rapidjson::Value(value.c_str(), allocator), //the player name also as string
            allocator
        );
    }
    content.AddMember("player_names", playerNamesJson, allocator);

    content.AddMember("player_count", player_count, allocator);
    content.AddMember("durak", durak, allocator);

};

void PlayerUpdate::fromJson(const rapidjson::Value& obj) {
    if(obj.HasMember("player_names") && obj["player_names"].IsObject()){
        const rapidjson::Value& playerNamesJson = obj["player_names"];
        for(auto itr = playerNamesJson.MemberBegin(); itr != playerNamesJson.MemberEnd(); ++itr){
            const std::string keyString = itr->name.GetString();
            if(!keyString.empty() && std::all_of(keyString.begin(), keyString.end(), ::isdigit)){
                unsigned int key = std::stoi(keyString);
                player_names[key] = itr->value.GetString();
            }else{
                std::cerr << "invalid key in player_names: "<< keyString << std::endl;
            }
        }
    } else{
        std::cerr << "Error: 'player names' is missing or not an object" << std::endl;
    }
    if(obj.HasMember("player_count") && obj["player_count"].IsUint()){
        player_count = obj["player_count"].GetUint();   
    } else {
        std::cerr << "Error: 'player count' is missing or not unsigned int" <<std::endl;
    }

    if(obj.HasMember("durak") && obj["durak"].IsUint()){
        durak = obj["durak"].GetUint();   
    } else {
        std::cerr << "Error: 'durak' is missing or not unsigned int" <<std::endl;
    }

};

//BATTLE STATE UPDATE
BattleStateUpdate::BattleStateUpdate() {messageType = MESSAGETYPE_BATTLE_STATE_UPDATE;}

void BattleStateUpdate::getContent(rapidjson::Value &content, Allocator &allocator) const {
    
    //defender
    content.AddMember("defender", defender, allocator);

    //attacker
    rapidjson::Value attackersJson(rapidjson::kArrayType);
    for(const auto p : attackers){
        attackersJson.PushBack(p, allocator);
    }
    content.AddMember("attackers", attackersJson, allocator);

    //idle
    rapidjson::Value idleJson(rapidjson::kArrayType);
    for(const auto p : idle){
        idleJson.PushBack(p, allocator);
    }
    content.AddMember("idle", idleJson, allocator);
};

void BattleStateUpdate::fromJson(const rapidjson::Value& obj) {
    //defender
    defender = obj["defender"].GetUint();
    //attackers
    const rapidjson::Value& attackersJson = obj["attackers"];
    attackers.clear();
    for(rapidjson::SizeType i = 0; i < attackersJson.Size(); ++i){
        attackers.push_back(attackersJson[i].GetUint());
    }
    //idle
    const rapidjson::Value& idleJson = obj["idle"];
    idle.clear();
    for(rapidjson::SizeType i = 0; i < idleJson.Size(); ++i){
        idle.push_back(idleJson[i].GetUint());
    }
};

// AVAILABLE MES
AvailableActionUpdate::AvailableActionUpdate() {messageType = MESSAGETYPE_AVAILABLE_ACTION_UPDATE;}

void AvailableActionUpdate::getContent(rapidjson::Value &content, Allocator &allocator) const {
    content.AddMember("pass_on", pass_on, allocator);
    content.AddMember("ok", ok, allocator);
    content.AddMember("pick_up", pick_up, allocator);
};

void AvailableActionUpdate::fromJson(const rapidjson::Value& obj) {
    if(obj.HasMember("pass_on") && obj["pass_on"].IsBool()){
        pass_on = obj["pass_on"].GetBool();
    }else{
        std::cerr << "Error: 'pass_on' is missing or not boolean." <<std::endl;
    }

    if(obj.HasMember("ok") && obj["ok"].IsBool()){
        ok = obj["ok"].GetBool();
    }else{
        std::cerr << "Error: 'ok' is missing or not boolean." <<std::endl;
    }
    if(obj.HasMember("pick_up") && obj["pick_up"].IsBool()){
        pick_up = obj["pick_up"].GetBool();
    }else{
        std::cerr << "Error: 'pick_up' is missing or not boolean." <<std::endl;
    }
};

GameStateUpdate::GameStateUpdate() {messageType = MESSAGETYPE_GAME_STATE_UPDATE;}
void GameStateUpdate::getContent(rapidjson::Value &content, Allocator &allocator) const {
    content.AddMember("state", (int) state, allocator);
}
void GameStateUpdate::fromJson(const rapidjson::Value& obj) {
    if(obj.HasMember("state") && obj["state"].IsInt()){
        state = (GameState) obj["state"].GetInt();

    } else std::cerr << "Error: 'state' is missing or not in the string." << std::endl;
}



PlayCardEvent::PlayCardEvent() {messageType = MESSAGETYPE_PLAYCARD_EVENT;}
void PlayCardEvent::getContent(rapidjson::Value &content, Allocator &allocator) const {
    rapidjson::Value cardsJson(rapidjson::kArrayType);
    for(const auto card : cards){
        cardsJson.PushBack(card.toInt(), allocator);
    }
    content.AddMember("cards", cardsJson, allocator);
    content.AddMember("slot", (int) slot, allocator);
}
void PlayCardEvent::fromJson(const rapidjson::Value& obj) {
    const rapidjson::Value& cardsJson = obj["cards"];
    cards.clear();
    for(rapidjson::SizeType i = 0; i < cardsJson.Size(); ++i){
        cards.insert(cardsJson[i].GetUint());
    }
    if(obj.HasMember("cards") && obj["cards"].IsArray()){
        const rapidjson::Value& cardsJson = obj["cards"];
        cards.clear();
        for(rapidjson::SizeType i = 0; i < cardsJson.Size(); ++i){
            uint cardInt = cardsJson[i].GetUint();
            cards.insert(cardInt);
        }
    }
    else{
        std::cerr << "Error: 'cards' are missing in play card event or not an array" << std::endl;
    }
    if(obj.HasMember("slot") && obj["slot"].IsInt()){
        slot = (CardSlot)obj["slot"].GetInt();

    } else{
        std::cerr << "Error: 'slot' is missing or not in the string." << std::endl;
        slot = CARDSLOT_1; //default card slot
    }
}


ClientActionEvent::ClientActionEvent() {messageType = MESSAGETYPE_CLIENT_ACTION_EVENT;}
void ClientActionEvent::getContent(rapidjson::Value &content, Allocator &allocator) const {
    content.AddMember("action", (int) action, allocator);
}

void ClientActionEvent::fromJson(const rapidjson::Value& obj) {
    if(obj.HasMember("action") && obj["action"].IsInt()){
        action = (ClientAction) obj["action"].GetInt();
    }
    else{
        std::cerr << "Error: 'action' is missing or not in the string." << std::endl;
        action = CLIENTACTION_OK; //default action
    }
}

ClientConnectEvent::ClientConnectEvent() {messageType = MESSAGETYPE_CLIENT_CONNECT_EVENT;}
void ClientConnectEvent::getContent(rapidjson::Value &content, Allocator &allocator) const {
    content.AddMember("username", rapidjson::Value(username.c_str(), allocator), allocator);
}
void ClientConnectEvent::fromJson(const rapidjson::Value& obj) {
    username = obj["username"].GetString();
}
    */

#endif
}