#include "oss_primitives.h"
#include "oss_vector.h"

/// Point intersection tests
bool OSS_Intersect(OSS_Vector point, OSS_Circle circle)
{
    return OSS_CalcMagnitudeSquared(point - (OSS_Vector){circle.x, circle.y}) <= circle.r * circle.r;
}
bool OSS_Intersect(OSS_Vector point, OSS_Line line)
{
    /// Treat the given point as if a point on the line,
    /// and check if it has the same Y-intercept as the point we know is definitely on the line
    float m = (line.u.y / line.u.x);
    return (point.y - (m * point.x)) == (line.p.y - (m * line.p.x));
}
bool OSS_Intersect(OSS_Vector point, OSS_LineSegment segment)
{
    /// Similar to above, but within a limited range
    return OSS_Intersect(point, (OSS_Line){segment.a, segment.b}) &&
     OSS_CalcDistanceSquared(segment.a, point) <= OSS_CalcDistanceSquared(segment.a, segment.b);
}
bool OSS_Intersect(OSS_Vector point, OSS_Ray ray)
{
    /// Same again, but this time we just check if the point is on the correct side
    return OSS_Intersect(point, (OSS_Line){ray.p, ray.u}) && OSS_CalcDotProduct(ray.u, point) > 0.0;
}
bool OSS_Intersect(OSS_Vector point, OSS_Rectangle rect)
{
    return point.x >= rect.x && point.x <= rect.xmax() && point.y >= rect.y && point.y <= rect.ymax();
}
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
