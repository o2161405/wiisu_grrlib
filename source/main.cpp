#include <grrlib.h>

#include <stdlib.h>
#include <stdio.h>
#include <wiiuse/wpad.h>
#include <math.h>

#include <fat.h>

#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <cstring>

#include <vector>
#include <memory>
#include <bitset>

#include <fstream>
#include <sstream>
#include <iostream>

#include "HitObject.hpp"
#include "Clock.hpp"
#include "Wiimote.hpp"

static u8 CalculateFrameRate(void);
static bool isHoveringOverLogo(int cursorX, int cursorY);
static void drawCursor(int x, int y);
static void InitializeGRRLIB(void);
static void GetSongFolder(void);

float wiisu_logo_scale = 1.0f;

enum class ScreenState {
    MENU,
    SONG_SELECT,
    GAME,
};

std::vector<std::string> songNames;
std::vector<std::string> artistNames;
std::vector<std::string> folderNames;

int songSelection = 0;

BeatmapClock beatmapclock;
BeatmapHandler beatmaphandler;

double approachRate = 9.5f;
double circleSize = 2.5f;
double overallDifficulty = 9.2f;
double hpDrainRate = 4.2f;

int main(int argc, char **argv) {
    bool ShowFPS = true;

    InitializeGRRLIB();
    fatInitDefault();

    #include "Assets.hpp"

    ScreenState CurrentScreen = ScreenState::MENU;

    Wiimote wiimote;

    GetSongFolder();

    while(1) {

        wiimote.Update();

        switch (CurrentScreen) {
            case ScreenState::MENU: {

                float normalized_logo_scale = 1 + (sin(wiisu_logo_scale) * 0.03f);

                GRRLIB_DrawImg(0, 0, Background, 0, 4, 4, 0xFFFFFFFF);
                GRRLIB_DrawImg(-172, -226, Background_Circles, wiisu_logo_scale/2, 1, 1, 0xFFFFFF20); // Rotates Slowly
                GRRLIB_DrawImg(0 - (wiimote.GetX() * 0.01), -118 - (wiimote.GetY() * 0.01), Background_Lines_Big, 0, 1, 1, 0xFFFFFFFF); // Parallax Effect
                GRRLIB_DrawImg(-65 - (wiimote.GetX() * 0.05), -82 - (wiimote.GetY() * 0.05), Background_Lines_Medium, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(-50 - (wiimote.GetX() * 0.1), -30 - (wiimote.GetY() * 0.1), Background_Lines_Small, 0, 1, 1, 0xFFFFFFFF);


                GRRLIB_DrawImg(5, 5, SettingsImage, 0, 0.8f, 0.8f, 0xFFFFFFFF);
                GRRLIB_DrawImg(120, 40, Wiisu, 0, normalized_logo_scale, normalized_logo_scale, 0xFFFFFFFF);

                if (isHoveringOverLogo(wiimote.GetX(), wiimote.GetY())) {

                    if (wiimote.IsButtonPressed(WPAD_BUTTON_A)) {
                        CurrentScreen = ScreenState::SONG_SELECT;
                        
                    }
                }

                wiisu_logo_scale += 0.015f;
                break;
            }

            case ScreenState::SONG_SELECT: {

                for (int i = 0; i < static_cast<int>(songNames.size()); i++) {
                    char buffer[255];

                    //snprintf(buffer, sizeof(buffer), "%s by %s", songNames[i].c_str(), artistNames[i].c_str());
                    snprintf(buffer, sizeof(buffer), "%s", folderNames[i].c_str());

                    GRRLIB_PrintfTTF(50, 0 + (i * 50), Font, buffer, 24, 0xFFFFFFFF);

                    if (i != songSelection) {
                        GRRLIB_Rectangle(0, 0 + (i * 50), 50, 10, 0xFFFFFFFF, false);
                    } else {
                        GRRLIB_Rectangle(0, 0 + (i * 50), 50, 10, 0xFFFFFFFF, true);
                    }

                }

                if (wiimote.IsButtonPressed(WPAD_BUTTON_DOWN)) songSelection += 1;

                if (wiimote.IsButtonPressed(WPAD_BUTTON_UP)) songSelection -= 1;

                if (wiimote.IsButtonPressed(WPAD_BUTTON_A)) {
                    // Replace this with the path to the song folder
                    //LoadBeatmap("sd:/apps/wiisu/Songs/1047286 Dance Gavin Dance - Son of Robot/Dance Gavin Dance - Son of Robot (Alumetri) [Catharsis].osu");
                    beatmaphandler.loadBeatmap("sd:/apps/wiisu/Songs/1047286 Dance Gavin Dance - Son of Robot/Dance Gavin Dance - Son of Robot (Alumetri) [Catharsis].osu");
                    CurrentScreen = ScreenState::GAME;
                    beatmapclock.start();
                }

                break;
            }

            case ScreenState::GAME: {

                beatmapclock.update();
                int currentTime = beatmapclock.getTime();

                for (int i = 0; i < static_cast<int>(beatmaphandler.getHitObjectCount()); i++) {
                    beatmaphandler.getHitObject(i)->draw(currentTime);
                }

                char buffer[255];
                snprintf(buffer, sizeof(buffer), "Current Song: %s by %s", songNames[songSelection].c_str(), artistNames[songSelection].c_str());
                GRRLIB_PrintfTTF(10, 10, Font, buffer, 12, 0xFFFFFFFF);
                char buffer2[255];
                snprintf(buffer2, sizeof(buffer2), "Time: %d", currentTime);
                GRRLIB_PrintfTTF(10, 25, Font, buffer2, 12, 0xFFFFFFFF);

                break;
            }

            default: {
                GRRLIB_PrintfTTFW(10, 10, Font, L"You haven't set a state for the game.", 24, 0xFFFFFFFF);
                break;
                }
        }

        drawCursor(wiimote.GetX(), wiimote.GetY());

        if (ShowFPS) {
            char FPS[255];
            snprintf(FPS, sizeof(FPS), "Current FPS: %d", CalculateFrameRate());
            GRRLIB_PrintfTTF(500 + 1, 25 + 1, Font, FPS, 12, 0x000000FF);
            GRRLIB_PrintfTTF(500, 25, Font, FPS, 12, 0xFFFFFFFF);
        }

        if (wiimote.IsButtonPressed(WPAD_BUTTON_HOME)) {
            break;
        }

        GRRLIB_Render();  
    }

    GRRLIB_FreeTTF(Font);
    GRRLIB_Exit();

    exit(0);
}

static u8 CalculateFrameRate(void) {
    static u8 frameCount = 0;
    static u32 lastTime;
    static u8 FPS = 0;
    const u32 currentTime = ticks_to_millisecs(gettime());

    frameCount++;
    if(currentTime - lastTime > 1000) {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
    return FPS;
}

static bool isHoveringOverLogo(int cursorX, int cursorY) {
    float distanceToMiddleX = abs(320 - cursorX);
    float distanceToMiddleY = abs(240 - cursorY);
    float distanceToCenter = sqrt((distanceToMiddleX * distanceToMiddleX) + (distanceToMiddleY * distanceToMiddleY));

    if (distanceToCenter < 200) {
        return true;
    }
    else {
        return false;
    }
}

static void drawCursor(int x, int y) {
    for (int i = 0; i < 3; i++) {
        GRRLIB_Circle(x, y, 12 + i, 0x00000010, true);
    }
    GRRLIB_Circle(x, y, 10, 0xEDE2B9FF, true);
}

static void InitializeGRRLIB(void) {
    GRRLIB_Init();
    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    GRRLIB_SetAntiAliasing(true);
}

static void GetSongFolder(void) {
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir("sd:/apps/wiisu/songs")) != NULL) {
        int i = 0;
        while ((ent = readdir(dir)) != NULL) {

            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                continue;
            }

            std::string entryName = ent->d_name;

            std::string songName = entryName.substr(entryName.find("-") + 2);
            songNames.push_back(songName);

            std::string artistName = entryName.substr(entryName.find(" ") + 1, entryName.find("-") - entryName.find(" ") - 1);
            artistNames.push_back(artistName);

            folderNames.push_back(entryName);

            i++;
        }
        closedir(dir);
    }
}