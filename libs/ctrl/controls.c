#include "controls.h"

#define CTRL_PI (3.141593f)

typedef struct LPYTCVec2Char
{
    char x, y;
} LPYTCVec2Char;

typedef struct LPYTControls
{
    SceCtrlData input;
    LPYTCVec2Char stick;
    unsigned int pressed;
    unsigned int held;
    unsigned int released;
    int hold_counter[32];
} LPYTControls;

static LPYTControls pad;

static void controls_AddInput(const unsigned int button)
{
    if((pad.input.Buttons & button) != 0)
	{
		pad.released &= ~button;
		
		if((pad.held & button) == 0)
		{
			pad.pressed |= button;
			pad.held |= button;
		}
	}
	else
	{
		if(pad.held & button)
			pad.released |= button;
		
		pad.pressed &= ~button;
		pad.held &= ~button;
	}
}

void controls_read()
{
    sceCtrlPeekBufferPositive(&pad.input, 1);

    pad.pressed = 0;
    pad.released = 0;

    //if (pad.input.Buttons & PSP_CTRL_HOME)
    //    return;

    pad.stick.x = (signed)pad.input.Lx - 128;
    pad.stick.y = (signed)pad.input.Ly - 128;

    controls_AddInput(PSP_CTRL_SELECT);
    controls_AddInput(PSP_CTRL_START);
    controls_AddInput(PSP_CTRL_UP);
    controls_AddInput(PSP_CTRL_RIGHT);
    controls_AddInput(PSP_CTRL_DOWN);
    controls_AddInput(PSP_CTRL_LEFT);
    controls_AddInput(PSP_CTRL_LTRIGGER);
    controls_AddInput(PSP_CTRL_RTRIGGER);
    controls_AddInput(PSP_CTRL_TRIANGLE);
    controls_AddInput(PSP_CTRL_CIRCLE);
    controls_AddInput(PSP_CTRL_CROSS);
    controls_AddInput(PSP_CTRL_SQUARE);
    controls_AddInput(PSP_CTRL_HOME);
}

int controls_pressed(const unsigned int button)
{
    if((pad.pressed & button) == button)
	{	
        pad.pressed &= ~button;
        return 1;
    }

    return 0;
}

int controls_held(const unsigned int button)
{
    return ((pad.held & button) == button);
}

int controls_released(const unsigned int button)
{
    if ((pad.released & button) == button)
        return 1;

    return 0;
}

int controls_AnalogX()
{
    return pad.stick.x;
}

int controls_AnalogY()
{
    return pad.stick.y;
}

void controls_init()
{
    memset(&pad, 0, sizeof(LPYTControls));

    sceCtrlSetSamplingCycle(0);

    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    sceCtrlSetIdleCancelThreshold(25, 25);
}