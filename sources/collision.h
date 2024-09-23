#pragma once

#include "raylib.h"
#include <ccd/ccd.h>
#include <ccd/vec3.h>
#include <ccd/quat.h>

struct _ccd_obj_t {
    ccd_vec3_t pos;
    ccd_quat_t rot, rot_inv;
};
typedef struct _ccd_obj_t ccd_obj_t;

struct _ccd_box_t {
    ccd_obj_t o;
    ccd_real_t dim[3];
};
typedef struct _ccd_box_t ccd_box_t;

struct _ccd_cap_t {
    ccd_obj_t o;
    ccd_real_t radius;
    ccd_vec3_t axis;
    ccd_vec3_t p1;
    ccd_vec3_t p2;
};
typedef struct _ccd_cap_t ccd_cap_t;

struct _ccd_cyl_t {
    ccd_obj_t o;
    ccd_real_t radius;
    ccd_vec3_t axis;
    ccd_vec3_t p1;
    ccd_vec3_t p2;
};
typedef struct _ccd_cyl_t ccd_cyl_t;

struct _ccd_sphere_t {
    ccd_obj_t o;
    ccd_real_t radius;
};
typedef struct _ccd_sphere_t ccd_sphere_t;

struct _ccd_convex_t {
    ccd_obj_t o;
    Polyhedron convex;
};
typedef struct _ccd_convex_t ccd_convex_t;

struct _ccd_triangle_t {
    ccd_obj_t o;
    ccd_vec3_t vertices[3];
};
typedef struct _ccd_triangle_t ccd_triangle_t;

typedef struct dContactGeom {
    ccd_vec3_t pos;          /*< contact position*/
    ccd_vec3_t normal;          /*< normal vector*/
    ccd_real_t depth;       /*< penetration depth*/
    Shape *g1, *g2;       /*< the colliding geoms*/
    int side1, side2;      /*< (to be documented)*/
} dContactGeom;



/** Support functions */
void ccdSupportBox(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v);
void ccdSupportCap(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v);
void ccdSupportCyl(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v);
void ccdSupportSphere(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v);
void ccdSupportConvex(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v);
void ccdSupportTriangle(const void* obj, const ccd_vec3_t* _dir, ccd_vec3_t* v);