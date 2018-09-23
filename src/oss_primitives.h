#ifndef OSS_PRIMITIVES_H
#define OSS_PRIMITIVES_H

#include "oss_vector.h"

struct OSS_Circle
{
    float x;
    float y;
    float r;
};

struct OSS_Line
{
    OSS_Vector p;
    OSS_Vector u;
};

struct OSS_LineSegment
{
    OSS_Vector a;
    OSS_Vector b;
};

struct OSS_Ray
{
    OSS_Vector p;
    OSS_Vector u;
};

struct OSS_Rectangle
{
    float x;
    float y;
    float xmax;
    float ymax;
};

struct OSS_Triangle
{
    OSS_Vector a;
    OSS_Vector b;
    OSS_Vector c;
};

/// Now for the fun part... intersection tests
/// There is some duplication here, but it should make auto-complete nicer
/// Point intersection tests
bool OSS_Intersect(OSS_Vector point, OSS_Circle circle);
bool OSS_Intersect(OSS_Vector point, OSS_Line line);
bool OSS_Intersect(OSS_Vector point, OSS_LineSegment segment);
bool OSS_Intersect(OSS_Vector point, OSS_Ray ray);
bool OSS_Intersect(OSS_Vector point, OSS_Rectangle rect);
/*
bool OSS_Intersect(OSS_Vector point, OSS_Triangle triangle);

/// Circle intersection tests
bool OSS_Intersect(OSS_Circle circleA, OSS_Circle circleB);
bool OSS_Intersect(OSS_Circle circle, OSS_Line line);
bool OSS_Intersect(OSS_Circle circle, OSS_LineSegment segment);
bool OSS_Intersect(OSS_Circle circle, OSS_Ray ray);
bool OSS_Intersect(OSS_Circle circle, OSS_Rectangle rect);
bool OSS_Intersect(OSS_Circle circle, OSS_Triangle triangle);

/// Line intersection tests
bool OSS_Intersect(OSS_Line lineA, OSS_Line lineB);
bool OSS_Intersect(OSS_Line line, OSS_Circle circle);
bool OSS_Intersect(OSS_Line line, OSS_LineSegment segment);
bool OSS_Intersect(OSS_Line line, OSS_Ray ray);
bool OSS_Intersect(OSS_Line line, OSS_Rectangle rect);
bool OSS_Intersect(OSS_Line line, OSS_Triangle triangle);

/// Line segment intersection tests
bool OSS_Intersect(OSS_LineSegment segmentA, OSS_LineSegment segmentB);
bool OSS_Intersect(OSS_LineSegment segment, OSS_Circle circle);
bool OSS_Intersect(OSS_LineSegment segment, OSS_Line line);
bool OSS_Intersect(OSS_LineSegment segment, OSS_Ray ray);
bool OSS_Intersect(OSS_LineSegment segment, OSS_Rectangle rect);
bool OSS_Intersect(OSS_LineSegment segment, OSS_Triangle triangle);

/// Ray intersection tests
bool OSS_Intersect(OSS_Ray rayA, OSS_Ray rayB);
bool OSS_Intersect(OSS_Ray ray, OSS_Circle circle);
bool OSS_Intersect(OSS_Ray ray, OSS_Line line);
bool OSS_Intersect(OSS_Ray ray, OSS_LineSegment segment);
bool OSS_Intersect(OSS_Ray ray, OSS_Rectangle rect);
bool OSS_Intersect(OSS_Ray ray, OSS_Triangle triangle);

/// Rectangle intersection tests, AABB only
bool OSS_Intersect(OSS_Rectangle rectA, OSS_Rectangle rectB);
bool OSS_Intersect(OSS_Rectangle rect, OSS_Circle circle);
bool OSS_Intersect(OSS_Rectangle rect, OSS_Line line);
bool OSS_Intersect(OSS_Rectangle rect, OSS_LineSegment segment);
bool OSS_Intersect(OSS_Rectangle rect, OSS_Ray ray);
bool OSS_Intersect(OSS_Rectangle rect, OSS_Triangle triangle);

/// Triangle intersection tests
bool OSS_Intersect(OSS_Triangle triangleA, OSS_Triangle triangleB);
bool OSS_Intersect(OSS_Triangle triangle, OSS_Circle circle);
bool OSS_Intersect(OSS_Triangle triangle, OSS_Line line);
bool OSS_Intersect(OSS_Triangle triangle, OSS_LineSegment segment);
bool OSS_Intersect(OSS_Triangle triangle, OSS_Ray ray);
bool OSS_Intersect(OSS_Triangle triangle, OSS_Rectangle rect);
*/
#endif // OSS_PRIMITIVES_H
