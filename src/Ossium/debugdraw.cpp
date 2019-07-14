#include "debugdraw.h"

namespace Ossium
{

    inline namespace Graphics
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
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Draw(Renderer& renderer) not implemented for debug graphic!");
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
            text->position = position;
            text->SetColor(color);
            if (backgroundColor.a != 0)
            {
                text->SetBackgroundColor(backgroundColor);
                text->SetBox(true);
            }
            text->SetText(str);
            text->TextToTexture(renderer, font, fontSize);
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

        void DebugDraw::Clear(StrID tag)
        {
            if (tag == nullptr)
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
                    SDL_Log("Could not find any graphics with tag %s!", tag);
                }
            }
        }

    }

}
