#pragma once

/*
    DURAK ONLINE NETWORK PROTOCOL

    das nur ändere wenns abgsproche isch
    das isch s most upstream stück code

    momentant muss es aber no überarbeitet werde
*/

#define uint unsigned int

#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <memory>
#include <set>
#include <map>
#include <list>
#include <unordered_set>

//  BASIC TYPES

//das sött mer no ändere
#define ClientID uint

enum Suit {
    SUIT_CLUBS,
    SUIT_SPADES,
    SUIT_DIAMONDS,
    SUIT_HEARTS,
    SUIT_count,
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
    RANK_count,
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

enum GameState {
    GAMESTATE_LOBBY,
    GAMESTATE_GAME,
    GAMESTATE_SPECTATOR,
    GAMESTATE_GAME_OVER,
    GAMESTATE_DURAK_SCREEN,
    GAMESTATE_LOGIN_SCREEN //only for client use
};

enum ClientAction {
    CLIENTACTION_OK,
    CLIENTACTION_PASS_ON,
    CLIENTACTION_PICK_UP,
    CLIENTACTION_READY,
    CLIENTACTION_LOBBY
};

enum PlayerRole {
    ATTACKER,
    DEFENDER,
    CO_ATTACKER,
    IDLE,
    FINISHED
}; //max 6 players

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

namespace std {
    template <>
    struct hash<Card> {
        size_t operator()(const Card& card) const {
            return hash<int>()(static_cast<int>(card.toInt()));
        }
    };
}


// MESSAGES


//DIE SIND DEPRECATED DI MUSS MER UMSTRUKTURIERE
enum MessageType { 
    MESSAGETYPE_TEST,
    // Server:* to Client:MasterNode
    MESSAGETYPE_SEND_POPUP, // Server:Battle to Client:MasterNode, notifies the client that the move was illegal
    MESSAGETYPE_CARD_UPDATE, // Server:CardManager to Client:MasterNode, communicates the current status of the cards in play
    MESSAGETYPE_PLAYER_UPDATE, // Server:Game to Client:MasterNode, provides an update on the players in the game
    MESSAGETYPE_BATTLE_STATE_UPDATE, // Server:Battle to Client:MasterNode, contains info on player roles (attackers, defender, idle)
    MESSAGETYPE_AVAILABLE_ACTION_UPDATE, // Server:Battle to Client:MasterNode, tells the client what actions are available
    MESSAGETYPE_GAME_STATE_UPDATE, // Server:* to Client:MasterNode, update game screen (lobby, game, spectator, game over, Durak screen)
    MESSAGETYPE_READY_UPDATE,
    // Client:MasterNode to Server:*
    MESSAGETYPE_PLAYCARD_EVENT, // Client:MasterNode to Server:Server->Game->Battle, informs server that a player is trying to play a card
    MESSAGETYPE_CLIENT_ACTION_EVENT, // Client:MasterNode to Server:Server->Game->Battle, info about which client action was performed
    MESSAGETYPE_CLIENT_CONNECT_EVENT, // Client:MasterNode to Server:Server->Game, info about the player
    MESSAGETYPE_REMOTE_DISCONNECT_EVENT // Client:MasterNode to Server:Server->Game, informs server that a player has disconnected
};

typedef rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> Allocator;

struct Message {
    virtual ~Message() = default; // i dont know if this is needed but like this i dont get errors on Mac
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

struct ReadyUpdate : public Message {
    ReadyUpdate();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::set<ClientID> players;
};

struct RemoteDisconnectEvent : public Message {
    RemoteDisconnectEvent();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);
};

// send an error message to the player that the move was illegal
struct PopupNotify : public Message {
    PopupNotify();
    void getContent(rapidjson::Value &content, Allocator &allocator) const;
    void fromJson(const rapidjson::Value& obj);

    std::string message;
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

#ifdef DURAK_PROTOCOL_IMPLEMENTATION

#include <algorithm> // for std::all_of for debugging purposes
#include <iostream>

std::unique_ptr<Message> deserialiseMessage(std::string string) {
    //std::cout << "Trying to deserialise Message: \n" << string << std::endl;
    
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

#endif