#pragma once

#include <memory>

class HitObject {
public:
    virtual ~HitObject() = default;

    virtual bool isHit(int cursorX, int cursorY) { return false; }

    virtual void draw() {}

    int x;
    int y;
    int time;
};

class Circle : public HitObject {
public:
    virtual ~Circle() = default;

    bool isHit(int cursorX, int cursorY) override {
        return false;
    }

    void draw() override {
        GRRLIB_Circle(x + 64, y + 48, 10, 0xED6911FF, true);
    }
};

class Slider : public HitObject {
public:
    virtual ~Slider() = default;

    bool isHit(int cursorX, int cursorY) override {
        return false;
    }

    void draw() override {
        GRRLIB_Circle(x + 64, y + 48, 10, 0x0EE3A3FF, true);
    }

    char curveType;
    std::vector<std::pair<int, int>> curvePoints;
};

std::vector<std::shared_ptr<HitObject>> hitObjects;