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

struct PlayerLobbyData {
    bool ready = false;
};

struct Player {
    ClientID id = 0;
    std::string name = "";
    bool durak = false;
    bool is_you = false;
    PlayerGameData* game;
    PlayerLobbyData* lobby;

    bool operator<(const Player& other) const;
    bool operator==(const Player& other) const;
};

enum SortType{
    SORTTYPE_SUIT,
    SORTTYPE_ASCEND,
    SORTTYPE_TRUMP
};

inline std::string getPlayerStateIcon(PlayerState state) {
    switch (state) {
        case PLAYERSTATE_ATTACK:
            return CLIENT_RES_DIR + "icons/attack.png";
        case PLAYERSTATE_DEFEND:
            return CLIENT_RES_DIR + "icons/defend.png";
        case PLAYERSTATE_IDLE:
            return CLIENT_RES_DIR + "icons/watch.png";
        case PLAYERSTATE_NONE:
            return CLIENT_RES_DIR + "icons/none.png";
    }
    return "error";
}

inline Extends alignExtends(Extends ext, float x, float y, float width, float height) {
    return {
        ext.x + ext.width * x,
        ext.y + ext.height * y,
        ext.width * width,
        ext.height * height
    };
}

inline Extends applyBorder(Extends ext, float b) {
    return {
            ext.x + b,
            ext.y + b,
            ext.width - 2*b,
            ext.height - 2*b
    };
}

Extends computeCompactExtends(Extends ext, float height, float width);

inline void printExt(std::string name, Extends ext) {
    std::cout << "Extends("<<name<<") x: " << ext.x << "; y: " << ext.y
                << "; width: " << ext.width << "; height: " << ext.height << std::endl;
}

#define throwServerError(m) throw std::runtime_error(std::string("\nSERVER SIDE ERROR: ")+std::string(m)+std::string("\nat: ")+std::string(__FILE__)+std::string(" line: ")+std::to_string(__LINE__))
#define throwGenericError(m) throw std::runtime_error(std::string("\nGENERIC RUNTIME ERROR: ")+std::string(m)+std::string("\nat: ")+std::string(__FILE__)+std::string(" line: ")+std::to_string(__LINE__))
#define throwServerErrorIF(m, c) if(c) throwServerError(m)