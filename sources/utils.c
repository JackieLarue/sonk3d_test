#include "main.h"
#include <math.h>

float Saturate(float f) 
{
	return fminf(fmaxf(f, 0.0f), 1.0f);
}

Vector3 PlaneProject(Vector3 point, Vector3 nor)
{
	Vector3 unit = Vector3Normalize(nor);
	float ptpd = Vector3DotProduct(unit, point);
	return Vector3Subtract(point, Vector3Scale(unit, ptpd));
}

float Vector3SignedAngle(Vector3 from, Vector3 to, Vector3 up)
{
	Vector3 fromUnit = Vector3Normalize(from);
	Vector3 toUnit = Vector3Normalize(to);
	Vector3 upUnit = Vector3Normalize(up);

	Vector3 right = Vector3Normalize(Vector3CrossProduct(upUnit, from));
	float dot = Vector3DotProduct(fromUnit, toUnit);
	float temp = Vector3DotProduct(right, toUnit);
	float rdot = temp > 0.0f && 1.0f || temp < 0.0f && -1.0f || 0.0f;

	if (FloatEquals(rdot, 0.0f)) {
		rdot = 1.0f;
	} if (dot >= 1.0f) {
		return 0.0f;
	} else if (dot <= -1) {
		return -PI * rdot;
	}
	return acosf(dot) * rdot;
}

Vector3 Vector3DivideValue(Vector3 v, float div) 
{
	if (FloatEquals(div, 0.0f)) return Vector3Zero();
	return Vector3Scale(v, 1.0f / div);
}

Vector3 Vector3TripleCrossProduct() 
{
	return (Vector3) { 0 };
}

Vector3 GetUp(Quaternion rotation)
{
	return Vector3RotateByQuaternion((Vector3) { 0.0f, 1.0f, 0.0f }, rotation);
}

Vector3 GetLook(Quaternion rotation)
{
	return Vector3RotateByQuaternion((Vector3) { 0.0f, 0.0f, -1.0f }, rotation);
}

Vector3 GetRight(Quaternion rotation)
{
	return Vector3RotateByQuaternion((Vector3) { 1.0f, 0.0f, 0.0f }, rotation);
}

Vector3 GetCapsuleUp(Capsule cap) 
{
	Vector3 d = Vector3Subtract(cap.tip, cap.base);
	d = Vector3Normalize(d);
	return d;
}

bool SameDirection(const Vector3 direction, const Vector3 ao) 
{
	return Vector3DotProduct(direction, ao) > 0;
}

Quaternion FromToRotationQuat(Vector3 from, Vector3 to) 
{
	//Get our axis and angle
	Vector3 axis = Vector3CrossProduct(from, to);
	float angle = Vector3Angle(from, to);

	//Create quat from axis and angle
	if (angle <= -PI) {
		return QuaternionFromAxisAngle((Vector3) { 0.0f, 0.0f, 1.0f }, PI);
	} else if (!FloatEquals(Vector3Length(axis), 0.0f)) {
		return QuaternionFromAxisAngle(axis, angle);
	} else {
		return QuaternionIdentity();
	}
}