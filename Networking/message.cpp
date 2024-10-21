#include "message.hpp"
#include "util.hpp"

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
        default:
            std::cout << "ahhh irgend en messagetype fehlt no in message.cpp" << std::endl;
        break;
    }

    message->fromJson(content);
    return message;
}

//TEST MESSAGE
TestMessage::TestMessage() {
    messageType = MESSAGETYPE_TEST;
}

std::string TestMessage::toJson() const {
    rapidjson::Document document;
    document.SetObject();
    auto allocator = document.GetAllocator();

    rapidjson::Value content(rapidjson::kObjectType);
    content.AddMember("x", x, allocator);
    content.AddMember("y", y, allocator);
    content.AddMember("string", rapidjson::Value(string.c_str(), allocator) , allocator);

    document.AddMember("message_type", static_cast<uint>(messageType), allocator);
    document.AddMember("content", content, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
};

void TestMessage::fromJson(const rapidjson::Value& obj) {
    x = obj["x"].GetInt();
    y = obj["y"].GetInt();
    string = obj["string"].GetString();
};

//CLIENT DISCONNECT (dummy message only sent my networking therefore the content is empty)
ClientDisconnectEvent::ClientDisconnectEvent() {
    messageType = MESSAGETYPE_CLIENT_DISCONNECT_EVENT;
}

std::string ClientDisconnectEvent::toJson() const {
    rapidjson::Document document;
    document.SetObject();
    auto allocator = document.GetAllocator();

    rapidjson::Value content(rapidjson::kObjectType); //empty
    document.AddMember("message_type", static_cast<uint>(messageType), allocator);
    document.AddMember("content", content, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

void ClientDisconnectEvent::fromJson(const rapidjson::Value& obj) {}