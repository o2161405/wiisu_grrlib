#pragma once

#include <ogc/lwp_watchdog.h>

class BeatmapClock {
public:
    BeatmapClock() {
        isRunning = false;
        startTime = 0;
        currentTime = 0;
    }

    ~BeatmapClock() = default;

    void start() {
        if (!isRunning) {
            startTime = ticks_to_millisecs(gettime());
            currentTime = startTime;
            isRunning = true;
        }
    }

    void stop() {
		if (isRunning) {
			isRunning = false;
		}
	}

    void update() {
        if (isRunning) {
            currentTime = abs(static_cast<int>(startTime - ticks_to_millisecs(gettime()))) - 2000;
        }
    }

    int getTime() {
        return currentTime;
    }

private:
    int startTime;
    int currentTime;
    bool isRunning;
};
