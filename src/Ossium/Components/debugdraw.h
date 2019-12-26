/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2019 Tim Lane
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
#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

#include <Box2D/Common/b2Draw.h>

#include "../Core/stringintern.h"
#include "text.h"

namespace Ossium
{

    inline namespace Graphics
    {

        /// Forward declaration of physics debug drawing implementation.
        class DebugDrawBox2D;

        /// Abstract class for drawing a graphic
        class OSSIUM_EDL DebugGraphic
        {
        public:
            DebugGraphic(SDL_Color color = Colors::RED);
            virtual ~DebugGraphic();

            virtual void Draw(Renderer& renderer);

            void SetColor(SDL_Color color);

        protected:
            SDL_Color drawColor = Colors::RED;

        };

        class OSSIUM_EDL DebugLine : public DebugGraphic
        {
        public:
            DebugLine(Line l, SDL_Color color = Colors::RED);
            DebugLine(Point a, Point b, SDL_Color color = Colors::RED);

            void Draw(Renderer& renderer);

        protected:
            Line line;

        };

        class OSSIUM_EDL DebugText : public DebugGraphic
        {
        public:
            DebugText(string str, Point position, Entity* entity, Font* font, Renderer& renderer, int fontSize = 24, SDL_Color color = Colors::RED, SDL_Color backgroundColor = Colors::TRANSPARENT);
            ~DebugText();

            void Draw(Renderer& renderer);

            Text* text = nullptr;

        };

        class OSSIUM_EDL DebugDraw : public GraphicComponent
        {
        public:
            DECLARE_COMPONENT(GraphicComponent, DebugDraw);

            void OnCreate();

            void Render(Renderer& renderer);

            /// Registers a graphic to be drawn
            template<typename T>
            void Draw(string tag, T&& graphic)
            {
                taggedGraphics[tag].push_back(new T(graphic));
            }

            /// Clears all registered graphics. Pass in a tag to clear all registered graphics with a matching tag.
            void Clear(string tag = "");

            /// Used for debug drawing Box2D bits.
            DebugDrawBox2D* physics;

        private:
            /// Lookup table for graphics that are drawn on screen
            map<string, vector<DebugGraphic*>> taggedGraphics;

        };

        inline float PTM(float pixels)
        {
            return pixels * 0.02f;
        }

        inline float MTP(float metres)
        {
            return metres * (1.0f / 0.02f);
        }

        /// Physics rendering class, used by Box2D for debug drawing.
        class OSSIUM_EDL DebugDrawBox2D : public b2Draw
        {
        public:
            DebugDrawBox2D(Renderer* renderer);

            virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

            /// Draw a solid closed polygon provided in CCW order.
            virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

            /// Draw a circle.
            virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

            /// Draw a solid circle.
            virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

            /// Draw a line segment.
            virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

            /// Draw a transform. Choose your own length scale.
            /// @param xf a transform.
            virtual void DrawTransform(const b2Transform& xf);

            /// Draw a point.
            virtual void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

        private:
            Renderer* renderer;

            SDL_Color GetColorSDL(const b2Color& c);

        };

    }

}

#endif // DEBUGDRAW_H
