#pragma once

typedef enum Status
{
	Status_Ground = 0x1,
	Status_OnColli = 0x2,
	Status_Hurt = 0x4,
	Status_ObjectInteract = 0x8,
	Status_Unknown2 = 0x10,
	Status_Unknown3 = 0x20,
	Status_Unknown4 = 0x40,
	Status_Unknown5 = 0x80,
	Status_Ball = 0x100,
	Status_LightDash = 0x200,
	Status_Attack = 0x400,
	Status_HoldObject = 0x800,
	Status_DoNextAction = 0x1000,
	Status_OnPath = 0x2000,
	Status_DisableControl = 0x4000,
	Status_Unknown6 = 0x8000,
} Status;

typedef enum player_state
{
	IDLE,
	WALK,
	SKID,
	SPINDASH,
	ROLL,
	AIRBORNE,
	HOMING,
	BOUNCE,
	RAIL,
	LIGHTDASH,
	AIRKICK,
	HURT
} player_state;

typedef enum ColFlags
{
	ColFlags_Solid = 0x1,
	ColFlags_Water = 0x2,
	ColFlags_NoFriction = 0x4,
	ColFlags_NoAccel = 0x8,
	ColFlags_UseSkyDrawDist = 0x20,
	ColFlags_IncAccel = 0x80,
	ColFlags_Dig = 0x100,
	ColFlags_NoClimb = 0x1000,
	ColFlags_Stairs = 0x4000,
	ColFlags_Hurt = 0x10000,
	ColFlags_Tube = 0x20000,
	ColFlags_Footprints = 0x100000,
	ColFlags_Grounded = 0x800000,
	ColFlags_NoAlphaSortPls = 0x1000000,
	ColFlags_AlphaSortThing = 0x2000000,
	ColFlags_NoVertexBuffer = 0x4000000,
	ColFlags_Dynamic = 0x8000000,
	ColFlags_UseRotation = 0x10000000,
	ColFlags_Visible = 0x80000000
} ColFlags;