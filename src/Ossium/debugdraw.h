#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

#include "stringintern.h"
#include "primitives.h"
#include "text.h"

namespace Ossium
{

    inline namespace Graphics
    {

        /// Abstract class for drawing a graphic
        class DebugGraphic
        {
        public:
            DebugGraphic(SDL_Color color = Colors::RED);
            virtual ~DebugGraphic();

            virtual void Draw(Renderer& renderer);

            void SetColor(SDL_Color color);

        protected:
            SDL_Color drawColor = Colors::RED;

        };

        class DebugLine : public DebugGraphic
        {
        public:
            DebugLine(Line l, SDL_Color color = Colors::RED);
            DebugLine(Point a, Point b, SDL_Color color = Colors::RED);

            void Draw(Renderer& renderer);

        protected:
            Line line;

        };

        class DebugText : public DebugGraphic
        {
        public:
            DebugText(string str, Point position, Entity* entity, Font* font, Renderer& renderer, int fontSize = 24, SDL_Color color = Colors::RED, SDL_Color backgroundColor = Colors::TRANSPARENT);
            ~DebugText();

            void Draw(Renderer& renderer);

            Text* text = nullptr;

        };

        class DebugDraw : public GraphicComponent
        {
        public:
            DECLARE_COMPONENT(DebugDraw);

            void Render(Renderer& renderer);

            /// Registers a graphic to be drawn
            template<typename T>
            void Draw(StrID tag, T graphic)
            {
                taggedGraphics[tag].push_back(new T(graphic));
            }

            /// Clears all registered graphics. Pass in a StrID tag to clear all registered graphics with a matching tag.
            void Clear(StrID tag = nullptr);

        private:
            /// Lookup table for graphics that are drawn on screen
            map<StrID, vector<DebugGraphic*>> taggedGraphics;

        };

    }

}

#endif // DEBUGDRAW_H
