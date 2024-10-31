#pragma once 

#include <string>

struct Extends {
    float x;
    float y;
    float width;
    float height;
    float layer;

    bool operator==(const Extends& other) const;
    bool contains(float x, float y);
};

struct Player {
    ClientID id = 0;
    std::string name = "";
    bool durak = false;
    bool is_you = false;
};