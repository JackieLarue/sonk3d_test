#include "main.h"
#include "collision.h"

BoundingBox GetCapsuleAABB(Capsule cap) 
{
    BoundingBox bound = { 0 };
    bound.min = Vector3SubtractValue( Vector3Min(cap.base, cap.tip), cap.radius );
    bound.max = Vector3AddValue( Vector3Max(cap.base, cap.tip), cap.radius );
    return bound;
}

void HardcodedCollision(playerwk* pwp) 
{
    if (pwp->t.translation.y < 5.0f) 
    {
        pwp->t.translation.y = 5.0f;
        pwp->spd.y = 0.0f;
    }
}

Vector3 ProjectAndScale(Vector3 vec, Vector3 normal) 
{
    float mag = Vector3Length(vec);
    vec = Vector3Normalize(PlaneProject(vec, normal));
    vec = Vector3Scale(vec, mag);
    return vec;
}

int maxBounces = 5;
float skinWidth = 0.015f;
float maxSlopeAngle = 55.0f;

Vector3 CollideAndSlide(Vector3 vel, Vector3 pos, int depth, bool gravityPass, Vector3 velInit, playerwk *pwp)
{
    if (depth >= maxBounces) {
        return Vector3Zero();
    }

	float dist = Vector3Length(vel) + skinWidth;
    CollPoints hit = { 0 };
    if (true) 
    {
        Vector3 snapToSurface = Vector3Scale(Vector3Normalize(vel), hit.depth - skinWidth);
        Vector3 leftOver = Vector3Subtract(vel, snapToSurface);
        float angle = Vector3Angle((Vector3){ 0.0f, 1.0f, 0.0f }, hit.normal);

        if (Vector3Length(snapToSurface) <= skinWidth) {
            snapToSurface = Vector3Zero();
        }

        if (angle <= maxSlopeAngle) 
        {
            if (gravityPass) {
                return snapToSurface;
            }
            leftOver = ProjectAndScale(leftOver, hit.normal);
        }
        else 
        {
            float scale = 1 - Vector3DotProduct(
                Vector3Normalize((Vector3) { hit.normal.x, 0.0f, hit.normal.z }), 
                Vector3Negate(Vector3Normalize((Vector3) { velInit.x, 0.0f, velInit.z }))
            );
            
            //temp
            bool isGrounded = false;
            if (pwp->attr == ColFlags_Grounded) {
                isGrounded = true;
            }
            if (isGrounded && !gravityPass) 
            {
                leftOver = Vector3Normalize(ProjectAndScale(
                    (Vector3) { leftOver.x, 0.0f, leftOver.z }, 
                    (Vector3) { hit.normal.x, 0.0f, hit.normal.z }));
                leftOver = Vector3Scale(leftOver, scale);
            }
            else {
                leftOver = Vector3Scale(ProjectAndScale(leftOver, hit.normal), scale);
            }
        }

        return Vector3Add(snapToSurface, CollideAndSlide(leftOver, Vector3Add(pos, snapToSurface), depth+1, gravityPass, velInit, pwp));
    }

    return vel;
}