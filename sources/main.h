#pragma once

#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include <ccd/ccd.h>
#include <ccd/vec3.h>

typedef struct PL_LANDPOSI
{
	float x;
	float y;
	float z;
	float r;
	float d;
	float h;
	int angy_dif;
	int angy_aim;
} PL_LANDPOSI;

typedef struct mtnjvwk
{
	__int16 mtnmode;
	__int16 jvmode;
	unsigned __int16 reqaction;
	unsigned __int16 action;
	unsigned __int16 lastaction;
	unsigned __int16 nextaction;
	unsigned __int16 acttimer;
	__int16 flag;
	float nframe;
	float start_frame;
	float* spdp;
	float* workp;
	//PL_ACTION* plactptr;
	//PL_JOIN_VERTEX* pljvptr;
	//NJS_ACTION* actwkptr;
} mtnjvwk;

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
	//bool jumpButton;
	InputState jump;
	InputState roll;
} SONIC_INPUT;

typedef struct Polyhedron 
{
	int vertexCount;
	Vector3 *vertices;
} Polyhedron;

typedef Transform OrientedBoundingBox;

typedef struct Sphere 
{
	Vector3 pos;
	float radius;
} Sphere;

typedef struct Capsule 
{
	Vector3 base;
	Vector3 tip;
	float radius;
} Capsule;

typedef enum ShapeType { POLYHEDRON, AABB, OBB, SPHERE, CAPSULE } ShapeType;
typedef struct Shape 
{
	ShapeType type;
	union 
	{
		Polyhedron p;
		BoundingBox bb;
		OrientedBoundingBox obb;
		Sphere s;
		Capsule c;
	};
} Shape;

typedef struct CollPoints 
{
	Vector3 a;
	Vector3 b;
	Vector3 normal;
	float depth;
	bool hasCollision;
} CollPoints;

typedef struct TimersAndSuch 
{
	float jump_timer;
	float spring_timer;
	float dashpanel_timer;
	float dashring_timer;
	float rail_debounce;
	float rail_trick;
	float spindash_speed;
} TimersAndSuch;

typedef struct playerwk
{
	float hpos;
	float dotp;
	Transform t;
	Vector3 eff;
	Vector3 acc;
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
	//csts* cstsp;
	//PL_FACE* pfp;
	//ModelStruct mS;
	//PL_LANDPOSI* island_list;
	player_parameter p;
	player_state pl_state;
	Capsule collider;
	//mtnjvwk mj;
	SONIC_INPUT input;
	Camera cam;
} playerwk;

//Vector3 Gravity;
//SONIC_INPUT input;

playerwk InitializePlayer();
void SonicTheHedgehog(playerwk* pwp);

void UpdateSonicInput(playerwk* pwp);
float GetAnalog_Turn(playerwk *pwp);
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
