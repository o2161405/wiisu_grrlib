#pragma once

#include <memory>

class HitObject {
public:
    int x;
    int y;
    int time;
    virtual ~HitObject() = default;

    virtual bool isHit(int cursorX, int cursorY) { return false; }

    virtual void draw() {}
};

class Circle : public HitObject {
public:
    virtual ~Circle() = default;

    bool isHit(int cursorX, int cursorY) override {
        return false;
    }

    void draw() override {
        GRRLIB_Circle(x + 64, y + 48, 10, 0xFFFFFFFF, true);
    }
};

class Slider : public HitObject {
public:
    virtual ~Slider() = default;

    bool isHit(int cursorX, int cursorY) override {
        return false;
    }

    void draw() override {
        GRRLIB_Circle(x + 64, y + 48, 10, 0xFFFFFFFF, true);
    }
};

std::vector<std::shared_ptr<HitObject>> hitObjects;