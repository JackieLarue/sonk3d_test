#pragma once

#include "main.h"

typedef struct InputState {
	bool down;
	bool press;
	bool release;
	int keyMap;
} InputState;

typedef struct PlayerControlState
{
	int gamepad;
	float stick_x;
	float stick_y;
	float stick_mag;
	float stick_angle;
	float stick_turn;
	//bool jumpButton;
	InputState jump;
	InputState roll;
} PlayerControlState;

typedef struct ControlState
{
	bool has_control;
	float stick_mag;
	float last_turn;
} ControlState;
