#include "main.h"
#include "collision.h"

#include "raylib.h"
#include "raymath.h"

#include <math.h>
#include <float.h>
#include <ccd/ccd.h>
#include <ccd/quat.h>

#define NUMC_MASK (0xffff)

void ccdSupportBox(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v)
{
    const ccd_box_t* o = (const ccd_box_t*)obj;
    ccd_vec3_t dir;

    ccdVec3Copy(&dir, _dir);
    ccdQuatRotVec(&dir, &o->o.rot_inv);

    ccdVec3Set(v, ccdSign(ccdVec3X(&dir)) * o->dim[0],
        ccdSign(ccdVec3Y(&dir)) * o->dim[1],
        ccdSign(ccdVec3Z(&dir)) * o->dim[2]);

    // transform support vertex
    ccdQuatRotVec(v, &o->o.rot);
    ccdVec3Add(v, &o->o.pos);
}

void ccdSupportCap(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v)
{
    const ccd_cap_t* o = (const ccd_cap_t*)obj;

    ccdVec3Copy(v, _dir);
    ccdVec3Scale(v, o->radius);

    if (ccdVec3Dot(_dir, &o->axis) > 0.0) {
        ccdVec3Add(v, &o->p1);
    }
    else {
        ccdVec3Add(v, &o->p2);
    }

}

void ccdSupportCyl(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v)
{
    const ccd_cyl_t* cyl = (const ccd_cyl_t*)obj;
    ccd_vec3_t dir;
    ccd_real_t len;

    ccd_real_t dot = ccdVec3Dot(_dir, &cyl->axis);
    if (dot > 0.0) {
        ccdVec3Copy(v, &cyl->p1);
    }
    else {
        ccdVec3Copy(v, &cyl->p2);
    }
    // project dir onto cylinder's 'top'/'bottom' plane
    ccdVec3Copy(&dir, &cyl->axis);
    ccdVec3Scale(&dir, -dot);
    ccdVec3Add(&dir, _dir);
    len = CCD_SQRT(ccdVec3Len2(&dir));
    if (!ccdIsZero(len)) {
        ccdVec3Scale(&dir, cyl->radius / len);
        ccdVec3Add(v, &dir);
    }
}

void ccdSupportSphere(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v)
{
    const ccd_sphere_t* s = (const ccd_sphere_t*)obj;

    ccdVec3Copy(v, _dir);
    ccdVec3Scale(v, s->radius);
    //dIASSERT(dFabs(CCD_SQRT(ccdVec3Len2(_dir)) - REAL(1.0)) < 1e-6); // ccdVec3Scale(v, CCD_ONE / CCD_SQRT(ccdVec3Len2(_dir)));

    ccdVec3Add(v, &s->o.pos);
}

void ccdSupportConvex(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v)
{
    const ccd_convex_t* c = (const ccd_convex_t*)obj;
    ccd_vec3_t dir, p;
    ccd_real_t maxdot, dot;
    int i;
    const ccd_real_t* curp;

    ccdVec3Copy(&dir, _dir);
    ccdQuatRotVec(&dir, &c->o.rot_inv);

    maxdot = -CCD_REAL_MAX;
    curp = c->convex.vertices;
    for (i = 0; i < c->convex.vertexCount; i++, curp += 3) {
        ccdVec3Set(&p, curp[0], curp[1], curp[2]);
        dot = ccdVec3Dot(&dir, &p);
        if (dot > maxdot) {
            ccdVec3Copy(v, &p);
            maxdot = dot;
        }
    }


    // transform support vertex
    ccdQuatRotVec(v, &c->o.rot);
    ccdVec3Add(v, &c->o.pos);
}

void ccdSupportTriangle(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v)
{
    const ccd_triangle_t* o = (ccd_triangle_t*)obj;
    ccd_real_t maxdot, dot;
    maxdot = -CCD_REAL_MAX;
    for (unsigned i = 0; i != 3; i++) {
        dot = ccdVec3Dot(_dir, &o->vertices[i]);
        if (dot > maxdot) {
            ccdVec3Copy(v, &o->vertices[i]);
            maxdot = dot;
        }
    }
}

void ccdCenter(const void* obj, ccd_vec3_t* c)
{
    const ccd_obj_t* o = (const ccd_obj_t*)obj;
    ccdVec3Copy(c, &o->pos);
}

int ccdCollide(
    Shape* o1, Shape* o2, int flags, dContactGeom* contact, int skip,
    void* obj1, ccd_support_fn supp1, ccd_center_fn cen1,
    void* obj2, ccd_support_fn supp2, ccd_center_fn cen2)
{
    ccd_t ccd = { 0 };
    int res;
    ccd_real_t depth;
    ccd_vec3_t dir, pos;

    int max_contacts = (flags & NUMC_MASK);

    if (max_contacts < 1) {
        return 0;
    }

    CCD_INIT(&ccd);
    ccd.support1 = supp1;
    ccd.support2 = supp2;
    ccd.center1 = cen1;
    ccd.center2 = cen2;
    ccd.max_iterations = 500;
    ccd.mpr_tolerance = (ccd_real_t)1E-6;


    if (flags & 1) { //if contact points unimportant
        if (ccdMPRIntersect(obj1, obj2, &ccd)) {
            return 1;
        }
        else {
            return 0;
        }
    }

    res = ccdMPRPenetration(obj1, obj2, &ccd, &depth, &dir, &pos);
    if (res == 0) {
        contact->g1 = o1;
        contact->g2 = o2;

        contact->side1 = contact->side2 = -1;

        contact->depth = depth;

        contact->pos.v[0] = ccdVec3X(&pos);
        contact->pos.v[1] = ccdVec3Y(&pos);
        contact->pos.v[2] = ccdVec3Z(&pos);

        ccdVec3Scale(&dir, -1.);
        contact->normal.v[0] = ccdVec3X(&dir);
        contact->normal.v[1] = ccdVec3Y(&dir);
        contact->normal.v[2] = ccdVec3Z(&dir);

        return 1;
    }

    return 0;
}

BoundingBox GetCapsuleAABB(Capsule cap) 
{
    BoundingBox bound = { 0 };
    bound.min = Vector3SubtractValue( Vector3Min(cap.base, cap.tip), cap.radius );
    bound.max = Vector3AddValue( Vector3Max(cap.base, cap.tip), cap.radius );
    return bound;
}

void HardcodedCollision(playerwk* pwp, BoundingBox floor) 
{
    BoundingBox playerBox = GetCapsuleAABB(pwp->collider);
    if (CheckCollisionBoxes(playerBox, floor)) 
    {

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
            if (pwp->attr == 4) {
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