#include <Networking/message.hpp>
#include <Networking/util.hpp>

std::unique_ptr<Message> deserialiseMessage(std::string string) {
    rapidjson::Document document;
    assert(!document.Parse(string.c_str()).HasParseError());

    MessageType type = static_cast<MessageType>(document["message_type"].GetInt());
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
        default:
            std::cout << "ahhh irgend en messagetype fehlt no in message.cpp" << std::endl;
        break;
    }

    message->fromJson(content);
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
    trump_suit = obj["trump_suit"].GetUint();

    //middlecards back to map
    const  rapidjson::Value& middleCardJson = obj["middle_cards"];
    for(auto itr = middleCardJson.MemberBegin(); itr != middleCardJson.MemberEnd(); ++itr){
        unsigned int slot = std::stoi(itr->name.GetString());
        unsigned int card = itr->value.GetUint();
        middle_cards[slot] = card;
    }

    const rapidjson::Value& handJson = obj["hand"];
    hand.clear(); // so that we only store the latest data
    for(rapidjson::SizeType i = 0; i < handJson.Size(); ++i){
        hand.push_back(handJson[i].GetUint());
    }
}


//PLAYER UPDATE
PlayerUpdate::PlayerUpdate() {messageType = MESSAGETYPE_TEST;}

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

    content.AddMember("number_players", number_players, allocator);
    content.AddMember("durak", durak, allocator);
};

void PlayerUpdate::fromJson(const rapidjson::Value& obj) {
    //TODO
}
