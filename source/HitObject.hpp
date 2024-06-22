#pragma once

class HitObject {
public:
    HitObject(int x, int y, int time, double CircleSize, double ApproachRate, double OverallDifficulty, double HPDrainRate)
        : x(x), y(y), time(time), CircleSize(CircleSize), ApproachRate(ApproachRate), OverallDifficulty(OverallDifficulty), HPDrainRate(HPDrainRate) {}
    virtual ~HitObject() = default;

    virtual bool isHit(int cursorX, int cursorY) {
        return false;
    }

    virtual void draw(int opacity) {}

    int getTime() {
        return time;
    }

    void setTime(int time) {
        this->time = time;
    }

    // I'm not proud of this
    int calculateOpacity(int currentTime) {

        int preempt = 0;
        if (ApproachRate < 5)       preempt = 1200 + 600 * (5 - ApproachRate) / 5;
		else if (ApproachRate == 5) preempt = 1200;
		else                        preempt = 1200 - 750 * (ApproachRate - 5) / 5;

		int fadeIn = 0;
        if (ApproachRate < 5)       fadeIn = 800 + 400 * (5 - ApproachRate) / 5;
        else if (ApproachRate == 5) fadeIn = 800;
		else                        fadeIn = 800 - 500 * (ApproachRate - 5) / 5;

        if (currentTime < time - preempt)               return 0;
        else if (currentTime > time + 2 * fadeIn / 3)   return 0;
		else if (currentTime < time - preempt + fadeIn) return 255 * (currentTime - time + preempt) / fadeIn;
		else                                            return 255;

	}

    double CircleSize;
    double ApproachRate;
    double OverallDifficulty;
    double HPDrainRate;

protected:
    int x;
    int y;
    int time;
};

class Circle : public HitObject {
public:
    Circle(int x, int y, int time, double CircleSize, double ApproachRate, double OverallDifficulty, double HPDrainRate)
        : HitObject(x, y, time, CircleSize, ApproachRate, OverallDifficulty, HPDrainRate) {}

    bool isHit(int cursorX, int cursorY) override {
        return false;
    }

    void draw(int currentTime) override {
        int RGBA = 0xFFFFFF00 + calculateOpacity(currentTime);
        GRRLIB_Circle(x + 64, y + 48, 10, RGBA, true);
    }
};

class Slider : public HitObject {
public:
    Slider(int x, int y, int time, double CircleSize, double ApproachRate, double OverallDifficulty, double HPDrainRate)
		: HitObject(x, y, time, CircleSize, ApproachRate, OverallDifficulty, HPDrainRate) {}
    ~Slider() = default;

    bool isHit(int cursorX, int cursorY) override {
        return false;
    }

    void draw(int opacity) override {
        switch (curveType) {
			case 'B':
				GRRLIB_Circle(x + 64, y + 48, 10, 0xFF0000FF, true);
				break;
			case 'L':
                GRRLIB_Circle(x + 64, y + 48, 10, 0x00FF00FF, true);
                for (size_t i = 0; i < curvePoints.size(); i++) {
                    auto& point = curvePoints[i];
                    GRRLIB_Circle(point.first + 64, point.second + 48, 5, 0x00FF00FF, true);
                    GRRLIB_Line(x + 64, y + 48, point.first + 64, point.second + 48, 0x00FF00FF);
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

    int SliderMultiplier;
    int SliderTickRate;

private:
    char curveType;
    std::vector<std::pair<int, int>> curvePoints;
};

std::vector<std::shared_ptr<HitObject>> hitObjects;