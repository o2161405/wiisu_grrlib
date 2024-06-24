#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

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

    int calculateOpacity(int currentTime) {

        // https://osu.ppy.sh/wiki/en/Beatmap/Approach_rate

        int preempt = 0;
        int fadeIn = 0;
        int absApproachRate = abs(ApproachRate - 5);

        if (ApproachRate < 5) {
            preempt = 1200 + 600 * absApproachRate / 5;
            fadeIn = 800 + 400 * absApproachRate / 5;
        } else if (ApproachRate == 5) {
            preempt = 1200;
            fadeIn = 800;
        } else {
            preempt = 1200 - 750 * absApproachRate / 5;
            fadeIn = 800 - 500 * absApproachRate / 5;
        }

        if (currentTime < time - preempt || currentTime > time + 2 * fadeIn / 3) {
            return 0;
        } else if (currentTime < time - preempt + fadeIn) {
            return 255 * (currentTime - time + preempt) / fadeIn;
        } else {
            return 255;
        }

    }

protected:
    int x;
    int y;
    int time;
    double CircleSize;
    double ApproachRate;
    double OverallDifficulty;
    double HPDrainRate;
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

    void setCurveType(char curveType) { this->curveType = curveType; }

    void addCurvePoint(int x, int y) { curvePoints.push_back(std::make_pair(x, y)); }

private:
    char curveType;
    std::vector<std::pair<int, int>> curvePoints;
};

class BeatmapHandler {
public:
    BeatmapHandler() {
        circleSize = 0;
        approachRate = 9.5;
        overallDifficulty = 0;
        hpDrainRate = 0;
    }

    std::vector<std::pair<int, int>> parseCurvePoints(const std::string &pointsString) {
        std::vector<std::pair<int, int>> curvePoints;
        std::istringstream iss(pointsString.substr(2)); // Remove the first 2 characters
        std::string token;
        while (std::getline(iss, token, '|')) {
            int x, y;
            sscanf(token.c_str(), "%d:%d", &x, &y);
            curvePoints.push_back(std::make_pair(x, y));
        }
        return curvePoints;
    }

    void loadBeatmap(const std::string &path) {

        hitObjects.clear();

        std::ifstream file(path);

        // Go to [HitObjects]
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("[HitObjects]") != std::string::npos) {
                break;
            }
        }

        // Add hit objects to vector
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            std::vector<std::string> tokens;
            while (std::getline(iss, token, ',')) {
                tokens.push_back(token);
            }

            int x = std::stoi(tokens[0]);
            int y = std::stoi(tokens[1]);
            int time = std::stoi(tokens[2]);
            int type = std::stoi(tokens[3]);

            if (type & 1) {
                hitObjects.push_back(std::make_shared<Circle>(x, y, time, circleSize, approachRate,
                        overallDifficulty, hpDrainRate));
            } else if (type & 2) {
                std::shared_ptr<Slider> slider = std::make_shared<Slider>(x, y, time, circleSize,
                        approachRate, overallDifficulty, hpDrainRate);

                slider->setCurveType(tokens[5][0]);
                std::vector<std::pair<int, int>> curvePoints = parseCurvePoints(tokens[5]);
                for (auto &curvePoint : curvePoints) {
                    slider->addCurvePoint(curvePoint.first, curvePoint.second);
                }

                hitObjects.push_back(slider);
            }
        }

        // Normalize hit objects time to 0
        int minTime = hitObjects[0]->getTime();
        for (auto &hitObject : hitObjects) {
            hitObject->setTime(hitObject->getTime() - minTime);
        }
    }

    int getHitObjectCount() {
        return hitObjects.size();
    }
    
    std::shared_ptr<HitObject> getHitObject(int index) {
        return hitObjects[index];
    }

private:
    std::vector<std::shared_ptr<HitObject>> hitObjects;
    double circleSize;
    double approachRate;
    double overallDifficulty;
    double hpDrainRate;
};