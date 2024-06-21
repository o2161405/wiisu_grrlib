#pragma once

class HitObject {
public:
    HitObject(int x, int y, int time) : x(x), y(y), time(time) {}
    virtual ~HitObject() = default;

    virtual bool isHit(int cursorX, int cursorY) {
        return false;
    }

    virtual void draw() {}

    int getTime() {
        return time;
    }

    void setTime(int time) {
        this->time = time;
    }

protected:
    int x;
    int y;
    int time;
};

class Circle : public HitObject {
public:
    Circle(int x, int y, int time) : HitObject(x, y, time) {}
    ~Circle() = default;

    bool isHit(int cursorX, int cursorY) override {
        return false;
    }

    void draw() override {
        GRRLIB_Circle(x + 64, y + 48, 10, 0xED6911FF, true);
    }
};

class Slider : public HitObject {
public:
    Slider(int x, int y, int time) : HitObject(x, y, time) {}
    ~Slider() = default;

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