#include "main.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>

Vector3 ToLocal(Vector3 gv, playerwk* pwp)
{
	return Vector3RotateByQuaternion(gv, QuaternionInvert(pwp->t.rotation));
}

Vector3 ToGlobal(Vector3 lv, playerwk* pwp) 
{
	return Vector3RotateByQuaternion(lv, pwp->t.rotation);
}

Vector3 PosToSpd(Vector3 pos) 
{
	return (Vector3){ -pos.z, pos.y, pos.x };
}

Vector3 SpdToPos(Vector3 spd)
{
	return (Vector3) { spd.z, spd.y, -spd.x };
}

Vector3 ToSpeed(playerwk* pwp, Vector3 vector) 
{
	Vector3 speed = ToLocal(vector, pwp);
	return PosToSpd(speed);
}

Vector3 FromSpeed(playerwk* pwp, Vector3 vector)
{
	Vector3 speed = ToGlobal(vector, pwp);
	return SpdToPos(speed);
}

float PGetDecel(float spd, float dec) 
{
	if (spd > 0.0f) {
		return -fminf(spd, -dec);
	} 
	else if (spd < 0.0f) {
		return fminf(-spd, dec);
	}
	return 0.0f;
}

void PSetAngle(Quaternion angle, playerwk* pwp) 
{
	if (pwp->attr == ColFlags_Grounded) {
		pwp->t.rotation = angle;
	} 
	else 
	{
		//Set angle, maintaining middle
		pwp->t.translation = Vector3Add(pwp->t.translation, Vector3Scale(GetUp(pwp->t.rotation), pwp->p.height * pwp->p.weight));
		pwp->t.rotation = angle;
		pwp->t.translation = Vector3Subtract(pwp->t.translation, Vector3Scale(GetUp(pwp->t.rotation), pwp->p.height * pwp->p.weight));
	}
	//Set other angle information
	pwp->dotp = Vector3DotProduct(GetUp(pwp->t.rotation), Vector3Normalize(pwp->gravity));
	pwp->floor_normal = GetUp(pwp->t.rotation);
}

float PTurn(float turn, playerwk* pwp) 
{
	pwp->t.rotation = QuaternionMultiply(pwp->t.rotation, QuaternionFromAxisAngle((Vector3) {0.0f, 1.0f, 0.0f}, turn));
	return turn;
}

//void OriginBasis(playerwk* pwp, Quaternion basis, float inertia)
//{
//	Vector3 prev_speed = ToSpeed(pwp->spd);
//	pwp->t.rotation = (basis * Basis).Orthonormalized();
//	Velocity = Velocity * inertia + FromSpeed(prev_speed) * (1.0f - inertia);
//}

//void OriginRotate(playerwk* pwp, Vector3 axis, float angle, float inertia) 
// {
//	OriginBasis(pwp, QuaternionFromAxisAngle(axis, angle), inertia);
// }

void PAdjustAngleYQ(float analog_turn, playerwk* pwp) 
{
	Vector3 prev_spd = ToGlobal(pwp->spd, pwp);
	analog_turn = Clamp(analog_turn, DEG2RAD * -45.0f, DEG2RAD * 45.0f);
	PTurn(analog_turn, pwp);
	
	pwp->spd = ToLocal(prev_spd, pwp);
}

void PAdjustAngleYS(float analog_turn, playerwk* pwp)
{
	Vector3 prev_spd = ToGlobal(pwp->spd, pwp);
	float max_turn = DEG2RAD * 1.40625f;
	Vector3 spd_scaled, prev_spd_scaled;
	
	if (pwp->spd.x > pwp->p.dash_speed)
		max_turn = fmaxf(max_turn - (sqrtf((pwp->spd.x - pwp->p.dash_speed) * 0.0625f)), 0.0f);
	
	analog_turn = Clamp(analog_turn, -max_turn, max_turn);
	PTurn(analog_turn, pwp);
	
	float inertia;
	if (pwp->dotp <= -0.4f) {
		inertia = 0.5f;
	} 
	else {
		inertia = 0.01f;
	}
	
	spd_scaled = Vector3Scale(pwp->spd, (1.0f - inertia));
	prev_spd_scaled = Vector3Scale(ToLocal(prev_spd, pwp), inertia);
	pwp->spd = Vector3Add(spd_scaled, prev_spd_scaled);
}

void PAdjustAngleY(float analog_turn, playerwk* pwp)
{
	bool has_control = GetAnalog(pwp);
	Vector3 prev_spd = ToGlobal(pwp->spd, pwp);
	float max_turn = fabsf(analog_turn);

	Vector3 spd_scaled, prev_spd_scaled;

	if (max_turn <= 45.0f * DEG2RAD) 
	{
		if (max_turn <= 22.5f * DEG2RAD) {
			max_turn /= 8.0f;
		} 
		else {
			max_turn /= 4.0f;
		}
	}
	else {
		max_turn = 11.25f * DEG2RAD;
	}
	
	PTurn(analog_turn, pwp);

	if (pwp->flag != Status_Ground|Status_OnColli) 
	{
		spd_scaled = Vector3Scale(pwp->spd, 0.1f);
		prev_spd_scaled = Vector3Scale(ToLocal(prev_spd, pwp), 0.9f);
	} 
	else 
	{
		float inertia;
		if (has_control) 
		{
			if (pwp->dotp <= 0.4f) {
				inertia = 0.5f;
			} 
			else {
				inertia = 0.99f;
			}
		}
		else {
			inertia = 0.05f;
		}

		if (pwp->frict_mult < 1.0f) {
			inertia *= pwp->frict_mult;
		}
		
		spd_scaled = Vector3Scale(pwp->spd, (1.0f - inertia));
		prev_spd_scaled = Vector3Scale(ToLocal(prev_spd, pwp), inertia);
	}
	pwp->spd = Vector3Add(spd_scaled, prev_spd_scaled);
}

//Rotation / turning
void PRotatedByGravity(playerwk* pwp)
{
	Vector3 a1a = ToGlobal(pwp->spd, pwp); //ToGlobal translates local space vector to global space
	Vector3 grav_norm = Vector3Normalize(pwp->gravity);
	float dotp = Vector3DotProduct(Vector3Normalize(a1a), grav_norm);

	if (Vector3Length(a1a) <= pwp->p.jog_speed || dotp >= -0.86f)
	{
		Vector3 a2a = ToLocal(grav_norm, pwp);

		if (a2a.y <= 0.0f && a2a.y > -0.87f)
		{
			//Get turn
			if (a2a.x < 0.0f) {
				a2a.x = -a2a.x;
			}

			float turn = -atan2f(a2a.z, a2a.x);

			//Get max turn
			if (a2a.z < 0.0f) {
				a2a.z = -a2a.z;
			}

			float max_turn;
			if (pwp->flag = Status_Ball) { 
				max_turn = a2a.z * (DEG2RAD * 16.875f);
			} 
			else {
				max_turn = a2a.z * (DEG2RAD * 8.4375f);
			}

			//Turn
			turn = Clamp(turn, -max_turn, max_turn);
			PTurn(turn, pwp);
		}
	}
}

void PRotatedByGravityS(playerwk *pwp)
{
	Vector3 a1a = ToGlobal(pwp->spd, pwp);
	Vector3 grav_norm = Vector3Normalize(pwp->gravity);

	if (Vector3Length(a1a) > pwp->p.jog_speed)
	{
		float dotp = Vector3DotProduct(Vector3Normalize(a1a), grav_norm);

		if (dotp > -0.86f)
		{
			Vector3 a2a = ToLocal(grav_norm, pwp);

			if (a2a.y > -0.87f) {
				//Get turn
				if (a2a.x < 0.0f) {
					a2a.x = -a2a.x;;
				}

				float turn = -atan2f(a2a.z, a2a.x);

				//Get max turn
				if (a2a.z < 0.0f) {
					a2a.z = -a2a.z;
				}

				float max_turn;
				if (pwp->flag == Status_Ball) {
					max_turn = fabsf((pwp->spd.x / pwp->p.jog_speed) * a2a.z * (DEG2RAD * 22.5f));
				} 
				else {
					max_turn = a2a.z * (DEG2RAD * 11.25f);
				}

				//Turn
				turn = Clamp(turn, -max_turn, max_turn);
				PTurn(turn, pwp);
			}
		}
	}
}

void PGetRotation(playerwk *pwp)
{
	//Get analogue state
	bool has_control = GetAnalog(pwp);
	float analogue_turn = GetAnalog_Turn(pwp);
	//float analogue_mag = GetAnalog_Mag();

	if (has_control)
	{
		PAdjustAngleY(analogue_turn, pwp);
	}
}

void AlignToGravity(playerwk* pwp)
{
	if (Vector3Length(pwp->spd) < pwp->p.dash_speed)
	{
		//Remember previous speed
		Vector3 prev_spd = ToGlobal(pwp->spd, pwp);

		//Angle
		Quaternion angle = pwp->t.rotation;

		//Get next angle
		Vector3 from = GetUp(angle);
		Vector3 to = Vector3Negate(Vector3Normalize(pwp->gravity));
		float turn = Vector3Angle(from, to);

		if (!FloatEquals(turn, 0.0f))
		{
			float max_turn = DEG2RAD * 11.25f;
			float lim_turn = Clamp(turn, -max_turn, max_turn);

			Quaternion next_ang = QuaternionMultiply(FromToRotationQuat(from, to), angle);

			PSetAngle(QuaternionLerp(angle, next_ang, lim_turn / turn), pwp);
		}

		//Keep using previous speed
		pwp->spd = ToLocal(prev_spd, pwp);
	}
}

//Acceleration / friction
void PGetBrake(playerwk* pwp)
{
	//Get physics values
	float weight = pwp->p.weight;

	//Get gravity force
	Vector3 acc = ToLocal(Vector3Scale(pwp->gravity, weight), pwp);

	//Air drag
	pwp->spd = Vector3Add(pwp->spd, Vector3Multiply(pwp->spd, (Vector3) { pwp->p.air_resist, pwp->p.air_resist_y, pwp->p.air_resist_z }));

	//Friction
	float x_frict = pwp->p.run_break * pwp->frict_mult;
	float z_frict = pwp->p.grd_frict_z * pwp->frict_mult;
	float x_accel = PGetDecel(pwp->spd.x + acc.x, x_frict);
	float z_accel = PGetDecel(pwp->spd.z + acc.z, z_frict);

	//Apply acceleration
	acc = Vector3Add(acc, (Vector3) { x_accel, 0.0f, z_accel });
	pwp->spd = Vector3Add(pwp->spd, acc);
}

void PGetInertia(playerwk* pwp) {
	//Gravity
	float weight = pwp->p.weight;
	Vector3 acc = Vector3Scale(ToLocal(pwp->gravity, pwp), weight);

	//Amplify gravity
	if (pwp->grounded && pwp->spd.x > pwp->p.run_speed && pwp->dotp < 0) {
		acc.y *= -8.0f;
	}

	//Air drag
	if (pwp->flag == Status_Ball && pwp->dotp < 0.98f) {
		acc.x += pwp->spd.x * -0.0002f;
	} 
	else {
		acc.x += pwp->spd.x * pwp->p.air_resist;
	}
	acc.y += pwp->spd.y * pwp->p.air_resist_y;
	acc.z += pwp->spd.z * pwp->p.air_resist_z;

	//Apply acceleration
	pwp->spd = Vector3Add(pwp->spd, acc);
}
