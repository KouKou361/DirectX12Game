#pragma once

#include "Lib.h"

struct Lib_CollisionRect
{
	VECTOR2 pos1;
	VECTOR2 pos2;
};
struct Lib_CollisionLine
{
	VECTOR2 start;
	VECTOR2 end;
};

struct Lib_CollisionCircle
{
	VECTOR3 pos;
	float radius;
};


class Lib_Collision
{
public:
	static bool RectVsLine(const Lib_CollisionRect& rect, const Lib_CollisionLine& line);
	static bool CircleVsCircle(const Lib_CollisionCircle& circle1, const Lib_CollisionCircle& circle2);
};