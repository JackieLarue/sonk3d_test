#pragma once

#include <raylib.h>
#include <raymath.h>

#include "enums.h"
#include "collision.h"
#include "input.h"

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

typedef struct player_parameter
{
	int jump2_timer;
	float pos_error;
	float lim_h_spd;
	float lim_v_spd;
	float max_x_spd;
	float max_psh_spd;
	float jmp_y_spd;
	float nocon_speed;
	float slide_speed;
	float jog_speed;
	float run_speed;
	float rush_speed;
	float crash_speed;
	float dash_speed;
	float jmp_addit;
	float run_accel;
	float air_accel;
	float slow_down;
	float run_break;
	float air_break;
	float air_resist_air;
	float air_resist;
	float air_resist_y;
	float air_resist_z;
	float grd_frict;
	float grd_frict_z;
	float lim_frict;
	float rat_bound;
	float rad;
	float height;
	float weight;
	float eyes_height;
	float center_height;
} player_parameter;

typedef struct AnimFrame {
	//Model model;
	//ModelAnimation* anims;
	int animsCount;
	unsigned int animIndex;
	unsigned int animCurrentFrame;
} AnimFrame;

typedef struct TimersAndSuch 
{
	int jump_timer;
	float spring_timer;
	float dashpanel_timer;
	float dashring_timer;
	float rail_debounce;
	float rail_trick;
	float spindash_speed;
} TimersAndSuch;

typedef struct playerwk
{
	//float hpos;
	float dotp;
	Transform t;
	//Vector3 eff;
	//Vector3 acc;
	Vector3 spd;
	Vector3 gravity;
	Vector3 wall_normal;
	Vector3 floor_normal;
	Vector3 last_up;
	unsigned int attr;
	unsigned int last_attr;
	bool grounded;
	unsigned int flag;
	float last_turn;
	float frict_mult;
	TimersAndSuch timer;
	//ModelStruct mS;
	player_parameter p;
	player_state pl_state;
	Capsule collider;
	SONIC_INPUT input;
	Camera cam;
} playerwk;

//Vector3 Gravity;
//SONIC_INPUT input;

playerwk InitializePlayer();
void SonicTheHedgehog(playerwk* pwp);

void UpdateSonicInput(playerwk* pwp);
float GetAnalog_Turn(playerwk* pwp);
float GetAnalog_Mag(playerwk* pwp); 
bool GetAnalog(playerwk* pwp); 

Vector3 ToLocal(Vector3 gv, playerwk *pwp);
Vector3 ToGlobal(Vector3 lv, playerwk *pwp);

float Saturate(float f);
Vector3 PlaneProject(Vector3 point, Vector3 nor);
float Vector3SignedAngle(Vector3 from, Vector3 to, Vector3 up);
Vector3 Vector3DivideValue(Vector3 v, float div);

Vector3 GetUp(Quaternion rotation);
Vector3 GetLook(Quaternion rotation);
Vector3 GetRight(Quaternion rotation);
Vector3 GetCapsuleUp(Capsule cap);
bool SameDirection(const Vector3 direction, const Vector3 ao);
Quaternion FromToRotationQuat(Vector3 from, Vector3 to);

Vector3 PosToSpd(Vector3 pos);
Vector3 SpdToPos(Vector3 spd);

float PTurn(float turn, playerwk* pwp);
void PAdjustAngleYQ(float analog_turn, playerwk* pwp);
void PAdjustAngleYS(float analog_turn, playerwk* pwp);
void PAdjustAngleY(float analog_turn, playerwk* pwp);

float PGetDecel(float spd, float dec);

BoundingBox GetCapsuleAABB(Capsule cap);
