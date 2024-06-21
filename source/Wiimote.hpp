#pragma once

#include <wiiuse/wpad.h>

class Wiimote {
public:
	Wiimote() {
        WPAD_Init();
        WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
        WPAD_SetVRes(0, 640, 480);
	}
	~Wiimote() = default;

	void Update() {
		WPAD_ScanPads();
        WPAD_IR(WPAD_CHAN_0, &this->WiimoteIR);
	}

	int GetX() {
		return WiimoteIR.x;
	}

	int GetY() {
		return WiimoteIR.y;
	}

	bool IsButtonPressed(u32 button) {
		return WPAD_ButtonsHeld(0) & button;
	}

private:
	ir_t WiimoteIR;

};
