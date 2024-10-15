#pragma once 

struct Extends {
    float x;
    float y;
    float width;
    float height;
    float layer;

    bool operator==(const Extends& other) const;
    bool contains(float x, float y);
};