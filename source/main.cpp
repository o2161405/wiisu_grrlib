#include <grrlib.h>

#include <stdlib.h>
#include <stdio.h>
#include <wiiuse/wpad.h>
#include <math.h>

#include <fat.h>

#include <sys/types.h>
#include <dirent.h>
#include <string>

#include <vector>
#include <memory>
#include <bitset>

#include "HitObject.hpp"
#include "Clock.hpp"

#define APPROACH_RATE_MS 400

static u8 CalculateFrameRate(void);
static bool isHoveringOverLogo(int cursorX, int cursorY);
static void drawCursor(int x, int y);
static void InitializeGRRLIB(void);
static void InitializeWPAD(void);
static void GetSongFolder(void);
static void LoadBeatmap(char* path);

float wiisu_logo_scale = 1.0f;
float normalized_logo_scale = 0.0f;

enum class ScreenState {
    MENU,
    SONG_SELECT,
    GAME,
};

std::vector<std::string> songNames;
std::vector<std::string> artistNames;
std::vector<std::string> folderNames;

int songSelection = 0;

Clock beatmapclock;

int main(int argc, char **argv) {
    bool ShowFPS = true;

    InitializeGRRLIB();
    InitializeWPAD();
    fatInitDefault();

    #include "Assets.hpp"

    ScreenState CurrentScreen = ScreenState::MENU;

    GetSongFolder();

    while(1) {

		WPAD_ScanPads();

        ir_t WiimoteIR;
        WPAD_IR(WPAD_CHAN_0, &WiimoteIR);

        switch (CurrentScreen) {
            case ScreenState::MENU: {

                float normalized_logo_scale = 1 + (sin(wiisu_logo_scale) * 0.03f);

                GRRLIB_DrawImg(0, 0, Background, 0, 4, 4, 0xFFFFFFFF);
                GRRLIB_DrawImg(-172, -226, Background_Circles, wiisu_logo_scale/2, 1, 1, 0xFFFFFF20); // Rotates Slowly
                GRRLIB_DrawImg(0 - (WiimoteIR.x * 0.01), -118 - (WiimoteIR.y * 0.01), Background_Lines_Big, 0, 1, 1, 0xFFFFFFFF); // Parallax Effect
                GRRLIB_DrawImg(-65 - (WiimoteIR.x * 0.05), -82 - (WiimoteIR.y * 0.05), Background_Lines_Medium, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(-50 - (WiimoteIR.x * 0.1), -30 - (WiimoteIR.y * 0.1), Background_Lines_Small, 0, 1, 1, 0xFFFFFFFF);


                GRRLIB_DrawImg(5, 5, SettingsImage, 0, 0.8f, 0.8f, 0xFFFFFFFF);
                GRRLIB_DrawImg(120, 40, Wiisu, 0, normalized_logo_scale, normalized_logo_scale, 0xFFFFFFFF);

                if (isHoveringOverLogo(WiimoteIR.x, WiimoteIR.y)) {

                    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A) {
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

                if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) songSelection += 1;

                if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) songSelection -= 1;

                if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_A) {
                    // Replace this with the path to the song folder
                    LoadBeatmap("sd:/apps/wiisu/Songs/1047286 Dance Gavin Dance - Son of Robot/Dance Gavin Dance - Son of Robot (Alumetri) [Catharsis].osu");
                    CurrentScreen = ScreenState::GAME;
                    beatmapclock.start();
				}

				break;
			}

            case ScreenState::GAME: {

                beatmapclock.update();
                int currentTime = beatmapclock.getTime();

                for (int i = 0; i < static_cast<int>(hitObjects.size()); i++) {
                    if (hitObjects[i]->getTime() < currentTime) {
						hitObjects[i]->draw();
					}
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

		drawCursor(WiimoteIR.x, WiimoteIR.y);

        if (ShowFPS) {
            char FPS[255];
            snprintf(FPS, sizeof(FPS), "Current FPS: %d", CalculateFrameRate());
            GRRLIB_PrintfTTF(500 + 1, 25 + 1, Font, FPS, 12, 0x000000FF);
            GRRLIB_PrintfTTF(500, 25, Font, FPS, 12, 0xFFFFFFFF);
        }

        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;

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

static void InitializeWPAD(void) {
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(0, 640, 480);
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

static void LoadBeatmap(char* path) {

    // Clear the hit objects vector
    hitObjects.clear();

    FILE* file = fopen(path, "r");

    int hitObjectIndex = 0;
    char line[255];

    // Skip to the HitObjects section
    while (fgets(line, 255, file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        if (strstr(line, "HitObjects") != NULL) {
            break;
        }
        hitObjectIndex++;
    }

    // Add hit objects to the vector
    while (fgets(line, sizeof(line), file)) {

        // Parse the line
		char* token = strtok(line, ",");
		int x = atoi(token);
		token = strtok(NULL, ",");
		int y = atoi(token);
		token = strtok(NULL, ",");
		int time = atoi(token);
        token = strtok(NULL, ",");
        int type = atoi(token);

        std::string binary = std::bitset<8>(type).to_string();
        if (binary[7] == '1') { // Circle
            std::shared_ptr<Circle> circle = std::make_shared<Circle>(x, y, time);
            hitObjects.push_back(circle);
        } else if (binary[6] == '1') { // Slider
            std::shared_ptr<Slider> slider = std::make_shared<Slider>(x, y, time);

            token = strtok(NULL, ",");
            slider->setCurveType(token[0]);

            while ((token = strtok(NULL, "|")) != NULL) {
                int pointX, pointY;
                sscanf(token, "%d:%d", &pointX, &pointY);
                slider->addCurvePoint(pointX, pointY);
            }

            hitObjects.push_back(slider);
        }

    }

    // Normalize time to 0
    int firstObjectTime = hitObjects[0]->getTime();
    for (int i = 0; i < static_cast<int>(hitObjects.size()); i++) {
        hitObjects[i]->setTime(hitObjects[i]->getTime() - firstObjectTime);
    }

	fclose(file);
}
