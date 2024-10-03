#pragma once

#include "raylib.h"

typedef struct Polyhedron
{
	int vertexCount;
	Vector3* vertices;
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