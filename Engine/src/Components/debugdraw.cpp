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
#include "../Core/physics.h"
#include "debugdraw.h"

using namespace std;

namespace Ossium
{

    /// DebugGraphic
    DebugGraphic::DebugGraphic(SDL_Color color)
    {
        drawColor = color;
    }

    DebugGraphic::~DebugGraphic()
    {
    }

    void DebugGraphic::Draw(Renderer& renderer)
    {
        Log.Error("Draw(Renderer& renderer) not implemented for debug graphic!");
    }

    void DebugGraphic::SetColor(SDL_Color color)
    {
        drawColor = color;
    }

    /// DebugLine
    DebugLine::DebugLine(Point a, Point b, SDL_Color color) : DebugGraphic(color)
    {
        line = Line(a, b);
    }

    DebugLine::DebugLine(Line l, SDL_Color color) : DebugGraphic(color)
    {
        line = l;
    }

    void DebugLine::Draw(Renderer& renderer)
    {
        line.Draw(renderer, drawColor);
    }

    /// DebugText
    DebugText::DebugText(string str, Point position, Entity* entity, Font* font, Renderer& renderer, int fontSize, SDL_Color color, SDL_Color backgroundColor) : DebugGraphic(color)
    {
        /// No need to register the text with the renderer, as the DebugDraw class gives us a renderer to use
        text = entity->AddComponent<Text>();
        text->GetTransform()->SetWorldPosition(position);
        text->layout.mainColor = color;
        if (backgroundColor.a != 0)
        {
            text->backgroundColor = backgroundColor;
            text->boxed = true;
        }
        text->text = str;
    }

    DebugText::~DebugText()
    {
    }

    void DebugText::Draw(Renderer& renderer)
    {
        text->Render(renderer);
    }

    /// DebugDraw
    REGISTER_COMPONENT(DebugDraw);

    void DebugDraw::OnCreate()
    {
        GraphicComponent::OnCreate();
        physics = new DebugDrawBox2D(GetService<Renderer>());
    }

    void DebugDraw::Render(Renderer& renderer)
    {
        for (auto i : taggedGraphics)
        {
            for (auto j : i.second)
            {
                j->Draw(renderer);
            }
        }
    }

    void DebugDraw::Clear(string tag)
    {
        if (tag.length() == 0)
        {
            for (auto i : taggedGraphics)
            {
                for (auto j : i.second)
                {
                    delete j;
                    j = nullptr;
                }
            }
            taggedGraphics.clear();
        }
        else
        {
            auto itr = taggedGraphics.find(tag);
            if (itr != taggedGraphics.end())
            {
                for (auto j : itr->second)
                {
                    delete j;
                    j = nullptr;
                }
                itr->second.clear();
            }
            else
            {
                Log.Warning("Could not find any graphics with tag {0}!", tag);
            }
        }
    }

    /// Box2D physics debug drawing
    DebugDrawBox2D::DebugDrawBox2D(Renderer* renderer)
    {
        this->renderer = renderer;
    }

    void DebugDrawBox2D::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        renderer->SetDrawColor(GetColorSDL(color));
        for (int i = 0, counti = vertexCount - 1; i < counti; i++)
        {
            SDL_RenderDrawLine(renderer->GetRendererSDL(), (int)MTP(vertices[i].x), (int)MTP(vertices[i].y), (int)MTP(vertices[i + 1].x), (int)MTP(vertices[i + 1].y));
        }
        SDL_RenderDrawLine(renderer->GetRendererSDL(), (int)MTP(vertices[vertexCount - 1].x), (int)MTP(vertices[vertexCount - 1].y), (int)MTP(vertices[0].x), (int)MTP(vertices[0].y));
    }

    void DebugDrawBox2D::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        DrawPolygon(vertices, vertexCount, color);
    }

    void DebugDrawBox2D::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
    {
        Circle c;
        c.x = MTP(center.x);
        c.y = MTP(center.y);
        c.r = MTP(radius);
        c.Draw(*renderer, GetColorSDL(color));
    }

    void DebugDrawBox2D::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
    {
        DrawCircle(center, radius, color);
    }

    void DebugDrawBox2D::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
    {
        Line l(Point(MTP(p1.x), MTP(p1.y)), Point(MTP(p2.x), MTP(p2.y)));
        l.Draw(*renderer, GetColorSDL(color));
    }

    void DebugDrawBox2D::DrawTransform(const b2Transform& xf)
    {
        Line up(Point(MTP(xf.p.x), MTP(xf.p.y)), Point(MTP(xf.p.x), MTP(xf.p.y - 5)));
        Line right(Point(MTP(xf.p.x), MTP(xf.p.y)), Point(MTP(xf.p.x + 5), MTP(xf.p.y)));
        up.Draw(*renderer, Colors::GREEN);
        right.Draw(*renderer, Colors::RED);
    }

    void DebugDrawBox2D::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
    {
        Rect debugPoint(MTP(p.x - size), MTP(p.y - size), MTP(size * 2), MTP(size * 2));
        debugPoint.DrawFilled(*renderer, GetColorSDL(color));
    }

    SDL_Color DebugDrawBox2D::GetColorSDL(const b2Color& c)
    {
        return {(Uint8)(c.r * 255.0f), (Uint8)(c.g * 255.0f), (Uint8)(c.b * 255.0f), (Uint8)(c.a * 255.0f)};
    }

}
