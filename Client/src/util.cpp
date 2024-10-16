#include "util.hpp"

bool Extends::operator==(const Extends& other) const {
    return (x == other.x && y == other.y && 
            width == other.width && height == other.height &&
            layer == other.layer);
}

bool Extends::contains(float x, float y) {
    if(x < this->x || y < this->y) return false;
    if(x > this->x + width || y > this->y + height) return false;
    return true;
}