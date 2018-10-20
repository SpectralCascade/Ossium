#include "primitives.h"
#include "vector.h"

namespace ossium
{
    /// Point intersection tests
    bool Intersect(Vector point, Circle circle)
    {
        return CalcMagnitudeSquared(point - (Vector){circle.x, circle.y}) <= circle.r * circle.r;
    }
    bool Intersect(Vector point, Line line)
    {
        /// Treat the given point as if a point on the line,
        /// and check if it has the same Y-intercept as the point we know is definitely on the line
        float m = (line.u.y / line.u.x);
        return (point.y - (m * point.x)) == (line.p.y - (m * line.p.x));
    }
    bool Intersect(Vector point, LineSegment segment)
    {
        /// Similar to above, but within a limited range
        return Intersect(point, (Line){segment.a, segment.b}) &&
         CalcDistanceSquared(segment.a, point) <= CalcDistanceSquared(segment.a, segment.b);
    }
    bool Intersect(Vector point, Ray ray)
    {
        /// Same again, but this time we just check if the point is on the correct side
        return Intersect(point, (Line){ray.p, ray.u}) && CalcDotProduct(ray.u, point) > 0.0;
    }
    bool Intersect(Vector point, Rectangle rect)
    {
        return point.x >= rect.x && point.x <= rect.xmax() && point.y >= rect.y && point.y <= rect.ymax();
    }
    /*
    bool Intersect(Vector point, Triangle triangle);

    /// Circle intersection tests
    bool Intersect(Circle circleA, Circle circleB);
    bool Intersect(Circle circle, Line line);
    bool Intersect(Circle circle, LineSegment segment);
    bool Intersect(Circle circle, Ray ray);
    bool Intersect(Circle circle, Rectangle rect);
    bool Intersect(Circle circle, Triangle triangle);

    /// Line intersection tests
    bool Intersect(Line lineA, Line lineB);
    bool Intersect(Line line, Circle circle);
    bool Intersect(Line line, LineSegment segment);
    bool Intersect(Line line, Ray ray);
    bool Intersect(Line line, Rectangle rect);
    bool Intersect(Line line, Triangle triangle);

    /// Line segment intersection tests
    bool Intersect(LineSegment segmentA, LineSegment segmentB);
    bool Intersect(LineSegment segment, Circle circle);
    bool Intersect(LineSegment segment, Line line);
    bool Intersect(LineSegment segment, Ray ray);
    bool Intersect(LineSegment segment, Rectangle rect);
    bool Intersect(LineSegment segment, Triangle triangle);

    /// Ray intersection tests
    bool Intersect(Ray rayA, Ray rayB);
    bool Intersect(Ray ray, Circle circle);
    bool Intersect(Ray ray, Line line);
    bool Intersect(Ray ray, LineSegment segment);
    bool Intersect(Ray ray, Rectangle rect);
    bool Intersect(Ray ray, Triangle triangle);

    /// Rectangle intersection tests, AABB only
    bool Intersect(Rectangle rectA, Rectangle rectB);
    bool Intersect(Rectangle rect, Circle circle);
    bool Intersect(Rectangle rect, Line line);
    bool Intersect(Rectangle rect, LineSegment segment);
    bool Intersect(Rectangle rect, Ray ray);
    bool Intersect(Rectangle rect, Triangle triangle);

    /// Triangle intersection tests
    bool Intersect(Triangle triangleA, Triangle triangleB);
    bool Intersect(Triangle triangle, Circle circle);
    bool Intersect(Triangle triangle, Line line);
    bool Intersect(Triangle triangle, LineSegment segment);
    bool Intersect(Triangle triangle, Ray ray);
    bool Intersect(Triangle triangle, Rectangle rect);
    */

}
