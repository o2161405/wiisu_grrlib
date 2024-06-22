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
        GRRLIB_Circle(x + 64, y + 48, 10, 0xFFFFFFFF, true);
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
        switch (curveType) {
			case 'B':
				GRRLIB_Circle(x + 64, y + 48, 10, 0xFF0000FF, true);
				break;
			case 'L':
				GRRLIB_Circle(x + 64, y + 48, 10, 0x00FF00FF, true);
                
                for (auto& point : curvePoints) {
					GRRLIB_Circle(point.first + 64, point.second + 48, 5, 0x00FF00FF, true);
				}

				break;
			case 'P':
				GRRLIB_Circle(x + 64, y + 48, 10, 0x0000FFFF, true);
				break;
			case 'C':
				GRRLIB_Circle(x + 64, y + 48, 10, 0xFFE600FF, true);
				break;
		}
    }

    void setCurveType(char curveType) {
		this->curveType = curveType;
	}

    void addCurvePoint(int x, int y) {
		curvePoints.push_back(std::make_pair(x, y));
	}

private:
    char curveType;
    std::vector<std::pair<int, int>> curvePoints;
};

std::vector<std::shared_ptr<HitObject>> hitObjects;