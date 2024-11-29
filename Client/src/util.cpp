#include "util.hpp"

bool Extends::operator==(const Extends& other) const {
    return (x == other.x && y == other.y && 
            width == other.width && height == other.height);
}

bool Extends::contains(float x, float y) {
    if(x < this->x || y < this->y) return false;
    if(x > this->x + width || y > this->y + height) return false;
    return true;
}

bool Player::operator<(const Player& other) const {
    return id < other.id;
}

bool Player::operator==(const Player& other) const {
    return id == other.id;
}

Extends computeCompactExtends(Extends ext, float height, float width) {
    float h, w;
    if( (float)ext.height / ext.width < (float)height / width ) { //height
        h = ext.height;
        w = (float) width * ext.height / height;
    } else { //width
        w = ext.width;
        h = (float) height * ext.width / width;
    }

    return {
        ext.x + (ext.width - w)/2,
        ext.y + (ext.height - h)/2,
        w,
        h
    };
}