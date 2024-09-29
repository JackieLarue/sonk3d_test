#include "main.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>

Vector3 PGetGravityAccel(playerwk* pwp) 
{
	float dotp = pwp->dotp;
	float spd_x = pwp->spd.x;

	//Get gravity force
	Vector3 acc = ToLocal(Vector3Scale(pwp->gravity, pwp->p.weight), pwp);

	//Get cross product between our moving velocity and floor normal
	Vector3 tnorm_cross_velocity = Vector3CrossProduct(ToGlobal(pwp->spd, pwp), pwp->floor_normal);
	
	if (dotp >= 0.1f || fabsf(tnorm_cross_velocity.y) <= 0.6f || spd_x < 1.16f)
	{
		if (dotp >= -0.4f || spd_x <= 1.16f)
		{
			if (dotp < -0.3f && spd_x > 1.16f) {
				acc.y += pwp->p.weight * -0.8f; //commented out by default
			}
			else if (dotp < -0.1f && spd_x > 1.16f) {
				acc.y += pwp->p.weight * -0.4f; //commented out by default
			}
			else if (dotp < 0.5f && fabsf(spd_x) < pwp->p.run_speed)
			{
				acc.x *= 4.225f;
				acc.z *= 4.225f;
			}
			else if (dotp >= 0.7f || fabsf(spd_x) > pwp->p.run_speed)
			{
				if (dotp >= 0.87f || pwp->p.jog_speed <= fabsf(spd_x)) {
					acc = acc; //commented out by default
				}
				else {
					acc.z *= 1.4f;
				}
			}
			else {
				acc.z *= 2.0f;
			}
		}
		else {
			acc.y += pwp->p.weight * -5.0f; //commented out by default
		}
	}
	else {
		acc = (Vector3){ 0.0f, -pwp->p.weight, 0.0f };
	}

	return acc;
}

float PGetAirResistAccelX(playerwk* pwp, float accel_x, bool has_control) 
{
	float spd_x = pwp->spd.x;
	float acc_x = accel_x;

	float max_x_spd = pwp->p.max_x_spd;

	if (has_control)
	{
		if (spd_x <= max_x_spd || pwp->dotp <= 0.96f)
		{
			if (spd_x > max_x_spd) {
				acc_x += (spd_x - max_x_spd) * pwp->p.air_resist;
			}
			else if (spd_x < 0.0f) {
				acc_x += spd_x * pwp->p.air_resist;
			}
		}
		else {
			acc_x += (spd_x - max_x_spd) * (pwp->p.air_resist * 1.7f);
		}
	}
	else
	{
		if (spd_x > pwp->p.run_speed) {
			acc_x += spd_x * pwp->p.air_resist;
		}
		else if (spd_x > max_x_spd) {
			acc_x += (spd_x - max_x_spd) * pwp->p.air_resist;
		}
		else if (spd_x < 0) {
			acc_x += spd_x * pwp->p.air_resist;
		}
	}

	return acc_x;
}

float PGetMoveAccel(playerwk* pwp, float analogue_mag)
{
	float move_accel;
	float spd_x = pwp->spd.x;

	//phys params
	float run_accel = pwp->p.run_accel;
	float jog_speed = pwp->p.jog_speed;
	float run_speed = pwp->p.run_speed;
	float rush_speed = pwp->p.rush_speed;

	//Get acceleration
	if (spd_x >= pwp->p.max_x_spd)
	{
		//Use lower acceleration if above max speed
		if (spd_x < pwp->p.max_x_spd || pwp->dotp >= 0.0f) {
			move_accel = run_accel * analogue_mag * 0.4f;
		}
		else {
			move_accel = run_accel * analogue_mag;
		}
	}
	else
	{
		//Get acceleration, stopping at intervals based on analogue stick magnitude
		move_accel = 0.0f;

		if (spd_x >= jog_speed)
		{
			if (spd_x >= run_speed)
			{
				if (spd_x >= rush_speed) {
					move_accel = run_accel * analogue_mag;
				}
				else if (analogue_mag <= 0.9f) {
					move_accel = run_accel * analogue_mag * 0.3f;
				}
				else {
					move_accel = run_accel * analogue_mag;
				}
			}
			else if (analogue_mag <= 0.7f)
			{
				if (spd_x < run_speed) {
					move_accel = run_accel * analogue_mag;
				}
			}
			else {
				move_accel = run_accel * analogue_mag;
			}
		}
		else if (analogue_mag <= 0.5f)
		{
			if (spd_x < (jog_speed + run_speed) * 0.5f) {
				move_accel = run_accel * analogue_mag;
			}
		}
		else {
			move_accel = run_accel * analogue_mag;
		}
	}

	return move_accel;
}

float PStickAccel(playerwk* pwp, float analogue_turn, float analogue_mag, float frict_mult)
{
	float move_accel = PGetMoveAccel(pwp, analogue_mag);

	//Turning
	float diff_angle = fabsf(analogue_turn);
	float forward_speed = pwp->spd.x;
	//float move_accel = move_acc;

	if (fabsf(forward_speed) < 0.001f && diff_angle > (DEG2RAD * 22.5f))
	{
		move_accel = 0.0;
		PAdjustAngleYQ(analogue_turn, pwp);
	}
	else
	{
		if (forward_speed < (pwp->p.jog_speed + pwp->p.run_speed) * 0.5f || diff_angle <= (DEG2RAD * 22.5f))
		{
			if (forward_speed < pwp->p.jog_speed || diff_angle >= (DEG2RAD * 22.5f))
			{
				if (forward_speed < pwp->p.dash_speed || pwp->attr != ColFlags_Grounded)
				{
					if (forward_speed >= pwp->p.jog_speed && forward_speed <= pwp->p.rush_speed && diff_angle > (DEG2RAD * 45.0f)) {
						move_accel *= 0.8f;
					}
					PAdjustAngleY(analogue_turn, pwp);
				}
				else {
					PAdjustAngleYS(analogue_turn, pwp);
				}
			}
			else {
				PAdjustAngleYS(analogue_turn, pwp);
			}
		}
		else
		{
			move_accel = pwp->p.slow_down / frict_mult;
			PAdjustAngleY(analogue_turn, pwp);
		}
	}

	return move_accel;
}

void PGetAcceleration(playerwk* pwp)
{
	//Get physics values
	float frict_mult = 1.0f;

	if (pwp->attr == ColFlags_Grounded) {
		frict_mult = pwp->frict_mult;
	}

	//Get our acceleration due to gravity
	Vector3 acc = PGetGravityAccel(pwp);

	//Get analogue state
	bool has_control = GetAnalog(pwp);
	float analogue_turn = GetAnalog_Turn(pwp);
	float analogue_mag = GetAnalog_Mag(pwp);

	//Air drag
	//X air drag
	acc.x = PGetAirResistAccelX(pwp, acc.x, has_control);

	//Y and Z air drag
	pwp->spd = Vector3Add(pwp->spd, Vector3Multiply(pwp->spd, (Vector3) { 0.0f, pwp->p.air_resist_y, pwp->p.air_resist_z }));

	float move_accel = 0.0f;

	//Movement
	if (has_control)
	{
		//Get acceleration
		move_accel = PStickAccel(pwp, analogue_turn, analogue_mag, frict_mult);
	}
	else 
	{
		//Decelerate
		move_accel = PGetDecel(pwp->spd.x + acc.x, pwp->p.slow_down);
	}

	//Apply movement acceleration
	acc.x += move_accel * frict_mult;

	//Apply acceleration
	pwp->spd = Vector3Add(pwp->spd, acc);
}

void PGetAirAcceleration(playerwk* pwp) {
	//Get analogue state
	bool has_control = GetAnalog(pwp);
	float analogue_turn = GetAnalog_Turn(pwp);
	float analogue_mag = GetAnalog_Mag(pwp);
	
	//Gravity
	pwp->spd = Vector3Add(pwp->spd, Vector3Scale(ToLocal(pwp->gravity, pwp), pwp->p.weight));
	
	//Air drag
	pwp->spd = Vector3Add(pwp->spd, Vector3Multiply(pwp->spd, Vector3DivideValue( (Vector3){ pwp->p.air_resist_air, pwp->p.air_resist_y, pwp->p.air_resist_z }, (1.0f + pwp->timer.rail_trick) )));

	//Use lighter gravity if A is held or doing a rail trick
	if ( (pwp->timer.rail_trick > 0.0f) || (pwp->timer.jump_timer > 0.0f && pwp->flag == Status_Ball && pwp->input.jump.down) ) {
		pwp->timer.jump_timer = max(pwp->timer.jump_timer - 1, 0);
		pwp->spd.y += pwp->p.jmp_addit * 0.8f * (1.0f + pwp->timer.rail_trick / 2.0f);
	}
	
	//Get our acceleration
	float accel;
	if (pwp->timer.rail_trick > 0.0f) 
	{
		//Constant acceleration
		accel = pwp->p.air_accel * (1.0f + pwp->timer.rail_trick / 2.5f);
		pwp->last_turn = 0.0f;
	}
	else if (!has_control) 
	{
		//No acceleration
		accel = 0.0f;
	}
	else
	{
		//Check if we should "skid"
		if ( (pwp->spd.x <= pwp->p.run_speed) || (fabsf(analogue_turn) <= DEG2RAD * 135.0f) ) 
		{
			if (fabsf(analogue_turn) <= DEG2RAD * 22.5f) 
			{
				if (pwp->spd.y >= 0) {
					accel = pwp->p.air_accel * analogue_mag;
				} else {
					accel = pwp->p.air_accel * 2.0f * analogue_mag;
				}
			}
			else {
				accel = 0.0f;
			}

			PAdjustAngleY(analogue_turn, pwp);
		}
		else
		{
			//Air brake
			accel = pwp->p.air_break * analogue_mag;
		}
	}
	
	//Accelerate
	pwp->spd.x += accel;
}