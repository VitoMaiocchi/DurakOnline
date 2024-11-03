#include <Networking/message.hpp>
#include <Networking/util.hpp>

#include <algorithm> // for std::all_of for debugging purposes

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
        case MESSAGETYPE_TEST:
            message = std::make_unique<TestMessage>();
        break;
        case MESSAGETYPE_CLIENT_DISCONNECT_EVENT:
            message = std::make_unique<ClientDisconnectEvent>();
        break;
        case MESSAGETYPE_ILLEGAL_MOVE_NOTIFY:
            message = std::make_unique<IllegalMoveNotify>();
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
            std::cout << "not implemented yet" << std::endl;
        break;
        case MESSAGETYPE_CLIENT_ACTION_EVENT:
            message = std::make_unique<ClientActionEvent>();
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
    auto allocator = document.GetAllocator();

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


//CLIENT DISCONNECT (dummy message; only sent my networking therefore the content is empty)
ClientDisconnectEvent::ClientDisconnectEvent() {messageType = MESSAGETYPE_CLIENT_DISCONNECT_EVENT;}
void ClientDisconnectEvent::getContent(rapidjson::Value &content, Allocator &allocator) const {}
void ClientDisconnectEvent::fromJson(const rapidjson::Value& obj) {}


//ILLEGAL MOVE NOTIFY
IllegalMoveNotify::IllegalMoveNotify(){messageType = MESSAGETYPE_ILLEGAL_MOVE_NOTIFY;}

void IllegalMoveNotify::getContent(rapidjson::Value &content, Allocator &allocator) const{
    content.AddMember("error", rapidjson::Value(error.c_str(), allocator), allocator);
}

void IllegalMoveNotify::fromJson(const rapidjson::Value& obj){
    error = obj["error"].GetString();
}

// CARD UPDATE
CardUpdate::CardUpdate(){messageType = MESSAGETYPE_CARD_UPDATE;}

void CardUpdate::getContent(rapidjson::Value &content, Allocator &allocator) const{
    
    //add opponent_cards which is a std::map
    // we iterate through the pairs and add each key-value pair
    //as a member of JSON
    rapidjson::Value opponentCardsJson(rapidjson::kObjectType);
    for(const auto &pair : opponent_cards){
        opponentCardsJson.AddMember(
            //player id (key) as string
            rapidjson::StringRef(std::to_string(pair.first).c_str()),
            rapidjson::Value(pair.second).Move(), //count
            allocator
        );
    }
    content.AddMember("opponent_cards", opponentCardsJson, allocator);

    content.AddMember("draw_pile_cards", draw_pile_cards, allocator);
    content.AddMember("trump_card", trump_card.toInt(), allocator);
    content.AddMember("trump_suit", trump_suit, allocator);

    rapidjson::Value middleCardJson(rapidjson::kObjectType);
    for(const auto &pair : middle_cards){
        middleCardJson.AddMember(
            // the slot as the key, as string
            rapidjson::StringRef(std::to_string(pair.first).c_str()),
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
    content.AddMember("state", ToInt(state), allocator);
}
void GameStateUpdate::fromJson(const rapidjson::Value& obj) {
    if(obj.HasMember("state") && obj["state"].IsInt()){
        state = FromInt<GameState>(obj["state"].GetInt());

    } else{
        std::cerr << "Error: 'state' is missing or not in the string." << std::endl;
        state = GAMESTATE_NONE;
    }
}



PlayCardEvent::PlayCardEvent() {

}
void PlayCardEvent::getContent(rapidjson::Value &content, Allocator &allocator) const {

}
void PlayCardEvent::fromJson(const rapidjson::Value& obj) {

}


ClientActionEvent::ClientActionEvent() {messageType = MESSAGETYPE_CLIENT_ACTION_EVENT;}
void ClientActionEvent::getContent(rapidjson::Value &content, Allocator &allocator) const {
    content.AddMember("action", ToInt(action), allocator);
}
void ClientActionEvent::fromJson(const rapidjson::Value& obj) {
    if(obj.HasMember("action") && obj["action"].IsInt()){
        action = FromInt<ClientAction>(obj["action"].GetInt());
    }
    else{
        std::cerr << "Error: 'action' is missing or not in the string." << std::endl;
        action = CLIENTACTION_OK; //default action
    }
}

ClientConnectEvent::ClientConnectEvent() {

}
void ClientConnectEvent::getContent(rapidjson::Value &content, Allocator &allocator) const {

}
void ClientConnectEvent::fromJson(const rapidjson::Value& obj) {

}


