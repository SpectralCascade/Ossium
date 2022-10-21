/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#include <cmath>

#include "funcutils.h"
#include "stringconvert.h"
#include "coremaths.h"
#include "shader.h"

using namespace std;

namespace Ossium
{

    ///
    /// Vector3
    ///
    Vector3::Vector3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector3 Vector3::Cross(const Vector3& vec)
    {
        return Vector3(
            y * vec.z - vec.y * z,
            z * vec.x - vec.z * x,
            x * vec.y - vec.x * y
        );
    }

    string Vector3::ToString()
    {
        return "(" + Utilities::ToString(x) + ", " + Utilities::ToString(y) + ", " + Utilities::ToString(z) + ")";
    }

    void Vector3::FromString(const string& str)
    {
        unsigned int len = str.length();
        if (len > 5)
        {
            /// Remove brackets
            string converted = str.substr(1, len - 2);
            /// Split and get the individual values
            string xhalf = SplitLeft(converted, ',');
            string yhalf = SplitRight(converted, ',');
            string zhalf = SplitRight(yhalf, ',');
            yhalf = SplitLeft(yhalf, ',');
            x = ToFloat(xhalf);
            y = ToFloat(yhalf);
            z = ToFloat(zhalf);
        }
    }

    ///
    /// Vector2
    ///

    Vector2::Vector2(const Vector3& vec)
    {
        x = vec.x;
        y = vec.y;
    }

    float Vector2::Dot(Vector2 vec)
    {
        return (x * vec.x) + (y * vec.y);
    }

    Vector2 Vector2::Normalized()
    {
        if (x == 0.0f && y == 0.0f)
        {
            return *this;
        }
        return (*this) * (1.0f / Length());
    }

    Vector2 Vector2::ProjectOnto(Vector2 vec)
    {
        return vec * (Dot(vec) / vec.LengthSquared());
    }

    float Vector2::DistanceSquared(Vector2 point)
    {
        point -= (*this);
        return point.LengthSquared();
    }

    float Vector2::Distance(Vector2 point)
    {
        point -= (*this);
        return point.Length();
    }

    float Vector2::LengthSquared() {
        return (x * x) + (y * y);
    }

    float Vector2::Length() {
        return sqrt(LengthSquared());
    }

    Vector2 Vector2::Reflection(Vector2 normal)
    {
        return (*this) - (ProjectOnto(normal) * 2.0f);
    }

    Vector2 Vector2::Rotation90Clockwise()
    {
        Vector2 output;
        output.x = y;
        output.y = -x;
        return output;
    }

    void Vector2::Rotate90Clockwise()
    {
        float cachex = x;
        x = y;
        y = -cachex;
    }

    Vector2 Vector2::Rotation90AntiClockwise()
    {
        return Vector2(-y, x);
    }

    void Vector2::Rotate90AntiClockwise()
    {
        float cachex = x;
        x = -y;
        y = cachex;
    }

    Vector2 Vector2::Lerp(Vector2 vec, float w)
    {
        w = Clamp(w, 0.0f, 1.0f);
        return ((*this) * (1.0f - w)) + (vec * w);
    }

    void Vector2::Rotate(float degrees)
    {
        RotateRad((Constants::pi / 180.0f) * degrees);
    }

    void Vector2::RotateRad(float radians)
    {
        float length = Length();
        float current = RotationRad();
        x = sinf(radians + current) * length;
        y = cosf(radians + current) * length;
    }

    Vector2 Vector2::Rotation(float degrees)
    {
        Vector2 v = *this;
        v.Rotate(degrees);
        return v;
    }

    Vector2 Vector2::RotationRad(float radians)
    {
        Vector2 v = *this;
        v.RotateRad(radians);
        return v;
    }

    Vector2 Vector2::Max(Vector2 vec)
    {
        return Vector2(max(x, vec.x), max(y, vec.y));
    }

    Vector2 Vector2::Min(Vector2 vec)
    {
        return Vector2(min(x, vec.x), min(y, vec.y));
    }

    string Vector2::ToString()
    {
        return "(" + Utilities::ToString(x) + ", " + Utilities::ToString(y) + ")";
    }

    void Vector2::FromString(const string& str)
    {
        unsigned int len = str.length();
        if (len > 4)
        {
            /// Remove brackets
            string converted = str.substr(1, len - 2);
            /// Split and get the individual values
            string xhalf = SplitLeft(converted, ',');
            string yhalf = SplitRight(converted, ',');
            x = ToFloat(xhalf);
            y = ToFloat(yhalf);
        }
    }


    Vector2 Vector2::operator*(float scalar) {
        return Vector2((Matrix<2, 1>)(*this) * scalar);
    }

    Vector2 Vector2::operator+(Vector2 operand) {
        return Vector2((Matrix<2, 1>)(*this) + operand);
    }

    Vector2 Vector2::operator-(Vector2 operand) {
        return Vector2((Matrix<2, 1>)(*this) - operand);
    }

    Vector2 operator*(float scalar, Vector2 operand)
    {
        return operand * scalar;
    }

    const Vector2 Vector2::Zero         =   {0, 0};
    const Vector2 Vector2::OneOne       =   {1, 1};
    const Vector2 Vector2::OneNegOne    =   {1, -1};
    const Vector2 Vector2::OneZero      =   {1, 0};
    const Vector2 Vector2::ZeroOne      =   {0, 1};
    const Vector2 Vector2::NegOneNegOne =   {-1, -1};
    const Vector2 Vector2::NegOneZero   =   {-1, 0};
    const Vector2 Vector2::ZeroNegOne   =   {0, -1};

    const Vector3 Vector3::Zeroes       =   {0, 0, 0};
    const Vector3 Vector3::Ones         =   {1, 1, 1};

    ///
    /// Rotation
    ///
/*
    Rotation::Rotation(const b2Rot& rot)
    {
        s = rot.s;
        c = rot.c;
    }

    float Rotation::GetDegrees()
    {
        return RadToDeg(GetAngle());
    }
    float Rotation::GetRadians()
    {
        return GetAngle();
    }
    void Rotation::SetDegrees(float angle)
    {
        Set(DegToRad(angle));
    }
    void Rotation::SetRadians(float angle)
    {
        Set(angle);
    }

    void Rotation::FromString(const string& data)
    {
        float degs;
        Utilities::FromString(degs, data);
        SetDegrees(degs);
    }

    string Rotation::ToString()
    {
        return Utilities::ToString(GetDegrees());
    }
*/
    ///
    /// Point
    ///

    Point::Point(float _x, float _y)
    {
        x = _x;
        y = _y;
    }

    Point::Point(const Vector2& vec)
    {
        x = vec.x;
        y = vec.y;
    }

    void Point::Draw(RenderInput* pass)
    {
        Renderer* renderer = pass->GetRenderer();

        // First, specify the layout of the data to pass to the GPU
        bgfx::VertexLayout layout;
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
        
        // Create the vertices
        float vertices[1][4] = {
            { round(x), round(y), 0.0f, (float)ColorToUint32(renderer->GetDrawColor(), SDL_PIXELFORMAT_ABGR8888) }
        };
        // Create index array
        uint16_t indices[] = { 0 };

        // Set primitive type to points
        renderer->SetState((renderer->GetState() & (~BGFX_STATE_PT_MASK)) | BGFX_STATE_PT_POINTS);
        bgfx::setState(renderer->GetState(), renderer->GetDrawColorUint32());

        // Create a VBO
        bgfx::VertexBufferHandle vbo = bgfx::createVertexBuffer(
            bgfx::copy(vertices, sizeof(vertices)),
            layout
        );

        // Create an IBO
        // TODO: check if this is necessary for points?
        bgfx::IndexBufferHandle ibo = bgfx::createIndexBuffer(bgfx::copy(indices, sizeof(indices)));

        // Load the vertex and fragment shaders
        // TODO load shaders elsewhere
        Shader vshader;
        vshader.LoadAndInit(Shader::GetPath("default.vert"));
        Shader fshader;
        fshader.LoadAndInit(Shader::GetPath("default.frag"));
        bgfx::ProgramHandle program = bgfx::createProgram(vshader.GetHandle(), fshader.GetHandle());

        // Setup transform and projection matrix
        Matrix<4, 4> view = Matrix<4, 4>::Identity();
        Matrix<4, 4> proj = Matrix<4, 4>::Identity();

        bgfx::setViewTransform(pass->GetID(), &view, &proj);
        bgfx::setVertexBuffer(0, vbo);
        bgfx::setIndexBuffer(ibo);

        // Submit the draw call
        bgfx::submit(pass->GetID(), program);

        bgfx::destroy(program);
    }

    void Point::Draw(RenderInput* pass, SDL_Color color)
    {
        pass->GetRenderer()->SetDrawColor(color);
        Draw(pass);
    }

    bool Point::Intersects(Circle circle)
    {
        return DistanceSquared((Vector2){circle.x, circle.y}) <= circle.r * circle.r;
    }
    bool Point::Intersects(InfiniteLine infiniteLine)
    {
        /// Treat this as a point on the line and check if it has the same Y-intercept
        float m = (infiniteLine.u.y / infiniteLine.u.x);
        return (y - (m * x)) == (infiniteLine.p.y - (m * infiniteLine.p.x));
    }
    bool Point::Intersects(Line line)
    {
        /// Similar to above, but within a limited range
        InfiniteLine infLine = {line.a, (Vector2)line.b};
        return Intersects(infLine) && line.a.DistanceSquared(*this) <= line.a.DistanceSquared(line.b);
    }
    bool Point::Intersects(Ray ray)
    {
        /// Same as infinite line but we also check if the point is on the correct side
        InfiniteLine line = {ray.p, static_cast<Point>(ray.u)};
        return Intersects(line) && ray.u.Dot(*this) > 0.0;
    }
    bool Point::Intersects(Rect rect)
    {
        return x >= rect.x && x < rect.xmax() && y >= rect.y && y < rect.ymax();
    }

    ///
    /// Ray
    ///

    Ray::Ray(Point origin, Vector2 direction)
    {
        p = origin;
        u = direction;
    }

    ///
    /// InfiniteLine
    ///

    InfiniteLine::InfiniteLine(Point p, Vector2 direction)
    {
        p = p;
        u = direction;
    }

    ///
    /// Circle
    ///

    /// TODO: use a more efficient drawing algorithm
    void Circle::Draw(RenderInput* pass, float smoothness)
    {
        int segments = r * r * Utilities::Clamp(smoothness);
        for (int i = 0; i < segments; i++)
        {
            Line(
                Point(x + r * sin(((Constants::pi * 2) / segments) * i), y + r * cos(((Constants::pi * 2) / segments) * i)),
                Point(x + r * sin(((Constants::pi * 2) / segments) * (i + 1)), y + r * cos(((Constants::pi * 2) / segments) * (i + 1)))
            ).Draw(pass);
        }
    }

    void Circle::Draw(RenderInput* pass, SDL_Color color, float smoothness)
    {
        pass->GetRenderer()->SetDrawColor(color);
        Draw(pass, smoothness);
    }

    bool Circle::Intersects(Circle circle)
    {
        float totalRadius = (r * circle.r);
        return Point(x, y).DistanceSquared(Point(circle.x, circle.y)) < totalRadius * totalRadius;
    }
    bool Circle::Intersects(InfiniteLine infiniteLine)
    {
        /// TODO
        return false;
    }
    bool Circle::Intersects(Line line)
    {
        /// TODO
        return false;
    }
    bool Circle::Intersects(Ray ray)
    {
        /// TODO
        return false;
    }
    bool Circle::Intersects(Rect rect)
    {
        Point offset;
        if (x < rect.x)
        {
            offset.x = rect.x;
        }
        else if (x > rect.xmax())
        {
            offset.x = rect.xmax();
        }
        else
        {
            offset.x = x;
        }
        if (y < rect.y)
        {
            offset.y = rect.y;
        }
        else if (y > rect.ymax())
        {
            offset.y = rect.ymax();
        }
        else
        {
            offset.y = y;
        }
        return (Point(x, y).DistanceSquared(offset) < r * r);
    }
    bool Circle::Contains(Point point)
    {
        return point.Intersects(*this);
    }

    ///
    /// Line
    ///

    Line::Line(Point start, Point end)
    {
        a = start;
        b = end;
    }

    // TODO sort this out...
    struct Vertex2D
    {
        float x;
        float y;
        Uint32 color0;

        static bgfx::VertexLayout Layout()
        {
            bgfx::VertexLayout layout;
            layout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
            return layout;
        }
    };

    void Line::Draw(RenderInput* pass)
    {
        Renderer* renderer = pass->GetRenderer();
        
        // Create the vertices
        auto color0 = renderer->GetDrawColorUint32();
        Vertex2D vertices[] = {
            { a.x, a.y, color0 },
            { b.x, b.y, color0 }
        };

        renderer->SetState(0
            // Draw lines rather than triangles or points
            | BGFX_STATE_PT_LINES
            // Write colour
            | BGFX_STATE_WRITE_RGB
            // Write alpha
            | BGFX_STATE_WRITE_A
            // Depth testing
            //| BGFX_STATE_WRITE_Z
            //| BGFX_STATE_DEPTH_TEST_LESS
            // Alpha opacity blending
            | OSSIUM_STANDARD_BLENDING
        );
        renderer->UpdateStateAndColor();

        // Create buffers
        bgfx::VertexBufferHandle vbo = bgfx::createVertexBuffer(
            bgfx::copy(vertices, sizeof(vertices)),
            Vertex2D::Layout()
        );

        // Load the vertex and fragment shaders
        // TODO load shaders elsewhere
        Shader vshader;
        vshader.LoadAndInit(Shader::GetPath("default.vert"));
        Shader fshader;
        fshader.LoadAndInit(Shader::GetPath("default.frag"));
        bgfx::ProgramHandle program = bgfx::createProgram(vshader.GetHandle(), fshader.GetHandle());

        // Setup transform and projection matrix
        Matrix<4, 4> view = Matrix<4, 4>::Identity();
        Matrix<4, 4> proj = Matrix<4, 4>::Orthographic(
            0, renderer->GetWidth(), renderer->GetHeight(), 0, 0, 100
        );

        bgfx::setViewTransform(pass->GetID(), &view, &proj);
        bgfx::setVertexBuffer(0, vbo);

        // Submit the draw call
        bgfx::submit(pass->GetID(), program);

        // TODO be less memory unfriendly
        // Destroy everything - bgfx only frees them after the next frame() call
        bgfx::destroy(vbo);

        bgfx::destroy(program);
    }

    void Line::Draw(RenderInput* pass, SDL_Color color)
    {
        pass->GetRenderer()->SetDrawColor(color);
        Draw(pass);
    }

    ///
    /// Rect
    ///

    Rect::Rect()
    {
    }

    Rect::Rect(float xpos, float ypos, float width, float height)
    {
        x = xpos;
        y = ypos;
        w = width;
        h = height;
    }

    Rect::Rect(const Vector2& position, const Vector2& dimensions)
    {
        x = position.x;
        y = position.y;
        w = dimensions.x;
        h = dimensions.y;
    }

    Rect::Rect(SDL_Rect rect)
    {
        x = (float)rect.x;
        y = (float)rect.y;
        w = (float)rect.w;
        h = (float)rect.h;
    }

    void Rect::DrawFilled(RenderInput* pass)
    {
        Renderer* renderer = pass->GetRenderer();
        
        // Create the vertices
        auto color0 = renderer->GetDrawColorUint32();
        Vertex2D vertices[] = {
            { x, y, color0 },
            { x + w, y, color0 },
            { x + w, y + h, color0 },
            { x, y + h, color0 }
        };
        // Create index array
        uint16_t indices[] = {
            0, 1, 2,
            0, 2, 3
        };

        // Draw triangles
        renderer->SetState(0
            // Write colour
            | BGFX_STATE_WRITE_RGB
            // Write alpha
            | BGFX_STATE_WRITE_A
            // Depth testing
            //| BGFX_STATE_WRITE_Z
            //| BGFX_STATE_DEPTH_TEST_LESS
            // Alpha opacity blending
            | OSSIUM_STANDARD_BLENDING
        );
        renderer->UpdateStateAndColor();

        // Create a VBO
        bgfx::VertexBufferHandle vbo = bgfx::createVertexBuffer(
            bgfx::copy(vertices, sizeof(vertices)), Vertex2D::Layout()
        );
        // Create an IBO
        bgfx::IndexBufferHandle ibo = bgfx::createIndexBuffer(bgfx::copy(indices, sizeof(indices)));

        // Load the vertex and fragment shaders
        // TODO load shaders elsewhere
        Shader vshader;
        vshader.LoadAndInit(Shader::GetPath("default.vert"));
        Shader fshader;
        fshader.LoadAndInit(Shader::GetPath("default.frag"));
        bgfx::ProgramHandle program = bgfx::createProgram(vshader.GetHandle(), fshader.GetHandle());

        // Setup transform and projection matrix
        Matrix<4, 4> view = Matrix<4, 4>::Identity();
        Matrix<4, 4> proj = Matrix<4, 4>::Orthographic(
            0, renderer->GetWidth(), renderer->GetHeight(), 0, 0, 100
        );

        bgfx::setViewTransform(pass->GetID(), &view, &proj);
        bgfx::setVertexBuffer(0, vbo);
        bgfx::setIndexBuffer(ibo);
        
        // Submit the draw call
        bgfx::submit(pass->GetID(), program);

        bgfx::destroy(program);
        bgfx::destroy(ibo);
        bgfx::destroy(vbo);
    }

    void Rect::DrawFilled(RenderInput* pass, SDL_Color color)
    {
        pass->GetRenderer()->SetDrawColor(color);
        DrawFilled(pass);
    }

    void Rect::Draw(RenderInput* pass)
    {
        Point vertices[4] = {
            Point(round(x), round(y)),
            Point(round(x + w), round(y)),
            Point(round(x + w), round(y + h)),
            Point(round(x), round(y + h))
        };
        for (unsigned int v = 0; v < 4; v++)
        {
            Line(vertices[v], vertices[v > 2 ? 0 : v + 1]).Draw(pass);
        }
    }

    void Rect::Draw(RenderInput* pass, SDL_Color color)
    {
        pass->GetRenderer()->SetDrawColor(color);
        Draw(pass);
    }

    bool Rect::Intersects(Circle circle)
    {
        return Point(x, y).Intersects(circle) || Point(x, ymax()).Intersects(circle) || Point(xmax(), y).Intersects(circle) || Point(xmax(), ymax()).Intersects(circle);
    }
    bool Rect::Intersects(InfiniteLine infiniteLine)
    {
        /// TODO
        return false;
    }
    bool Rect::Intersects(Line line)
    {
        /// TODO
        return false;
    }
    bool Rect::Intersects(Ray ray)
    {
        /// TODO
        return false;
    }
    bool Rect::Intersects(Rect rect)
    {
        return !((x > rect.xmax() || xmax() < rect.x) && (y > rect.ymax() || ymax() < rect.y));
    }
    bool Rect::Contains(Point point)
    {
        return point.Intersects(*this);
    }

    SDL_Rect Rect::SDL()
    {
        return (SDL_Rect){(int)round(x), (int)round(y), (int)round(w), (int)round(h)};
    }

    void Rect::FromString(const string& str)
    {
        string stripped = Utilities::Strip(Utilities::Strip(str, '('), ')');
        vector<string> split = Utilities::Split(stripped, ',');
        if (split.size() >= 4)
        {
            x = Utilities::ToFloat(split[0]);
            y = Utilities::ToFloat(split[1]);
            w = Utilities::ToFloat(split[2]);
            h = Utilities::ToFloat(split[3]);
        }
    }

    string Rect::ToString()
    {
        return "(" + Utilities::ToString(x) + ", " + Utilities::ToString(y) + ", " + Utilities::ToString(w) + ", " + Utilities::ToString(h) + ")";
    }

    const Rect Rect::Zero = {0, 0, 0, 0};

    bool Rect::operator==(const Rect& rect)
    {
        return rect.x == x && rect.y == y && rect.w == w && rect.h == h;
    }
    bool Rect::operator!=(const Rect& rect)
    {
        return !(*this == rect);
    }

    ///
    /// Polygon
    ///

    void Polygon::DrawFilled(RenderInput* pass)
    {
        /// TODO: this
    }

    void Polygon::DrawFilled(RenderInput* pass, SDL_Color color)
    {
        pass->GetRenderer()->SetDrawColor(color);
        DrawFilled(pass);
    }

    void Polygon::Draw(RenderInput* pass, SDL_Color color)
    {
        pass->GetRenderer()->SetDrawColor(color);
        Draw(pass);
    }

    void Polygon::Draw(RenderInput* pass)
    {
        if (!vertices.empty())
        {
            Point previousPoint = vertices[0];
            Line renderLine = Line(previousPoint, Vector2::Zero);
            for (unsigned int i = 1, counti = vertices.size(); i < counti; i++)
            {
                renderLine.b = Point(vertices[i].x, vertices[i].y);
                renderLine.Draw(pass);
                previousPoint = vertices[i];
            }
            renderLine.b = Point(vertices[0].x, vertices[0].y);
            renderLine.Draw(pass);
        }
    }

}
