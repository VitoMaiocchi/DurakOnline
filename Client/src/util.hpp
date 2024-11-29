#pragma once 

#include <string>
#include <exception>
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

enum PlayerState {
    PLAYERSTATE_IDLE,
    PLAYERSTATE_ATTACK,
    PLAYERSTATE_DEFEND,
    PLAYERSTATE_NONE
};

struct PlayerGameData {
    PlayerState state = PLAYERSTATE_NONE;
    int cards = -1;
};

struct Player {
    ClientID id = 0;
    std::string name = "";
    bool durak = false;
    bool is_you = false;
    PlayerGameData* game;

    bool operator<(const Player& other) const;
    bool operator==(const Player& other) const;
};


inline std::string getPlayerStateIcon(PlayerState state) {
    switch (state) {
        case PLAYERSTATE_ATTACK:
            return CLIENT_RES_DIR + "icons/attack.png";
        case PLAYERSTATE_DEFEND:
            return CLIENT_RES_DIR + "icons/defend.png";
        case PLAYERSTATE_IDLE:
            return CLIENT_RES_DIR + "icons/watch.png";
    }
    return "error";
}

Extends computeCompactExtends(Extends ext, float height, float width);

inline void printExt(std::string name, Extends ext) {
    std::cout << "Extends("<<name<<") x: " << ext.x << "; y: " << ext.y
                << "; width: " << ext.width << "; height: " << ext.height << std::endl;
}

#define throwServerError(m) throw std::runtime_error(std::string("\nSERVER SIDE ERROR: ")+std::string(m)+std::string("\nat: ")+std::string(__FILE__)+std::string(" line: ")+std::to_string(__LINE__))
#define throwGenericError(m) throw std::runtime_error(std::string("\nGENERIC RUNTIME ERROR: ")+std::string(m)+std::string("\nat: ")+std::string(__FILE__)+std::string(" line: ")+std::to_string(__LINE__))
#define throwServerErrorIF(m, c) if(c) throwServerError(m)