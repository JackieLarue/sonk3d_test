#pragma once

#include <main.h>

typedef struct {
	bool down;
	bool press;
	bool release;
	int keyMap;
} InputState;

typedef struct SONIC_INPUT
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
} SONIC_INPUT;

typedef struct ControlState
{
	bool has_control;
	float stick_mag;
	float last_turn;
} ControlState;
