#pragma once 

#include <string>
#include "../../Networking/include/Networking/util.hpp"

#define cast(type, ptr) dynamic_cast<type*>(ptr.get()) 
#define CARD_TEXTURE_WIDTH 500
#define CARD_TEXTURE_HEIGHT 726

struct Extends {
    float x;
    float y;
    float width;
    float height;
    float layer;  //TODO: ich bin geistig das isch komplett unnötig

    bool operator==(const Extends& other) const;
    bool contains(float x, float y);
};

struct Player {
    ClientID id = 0;
    std::string name = "";
    bool durak = false;
    bool is_you = false;
};