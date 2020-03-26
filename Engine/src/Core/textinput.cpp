#include "textinput.h"
#include "utf8.h"

namespace Ossium
{

    inline namespace Input
    {

        unsigned int TextInputHandler::numTextInputListeners = 0;

        ActionOutcome TextInputHandler::HandleInput(const SDL_Event& raw)
        {
            ActionOutcome result = ActionOutcome::Ignore;
            if (listenForTextInput && raw.type == SDL_TEXTINPUT)
            {
                // Collect input text
                text += raw.text.text;
                // If the user presses multiple keys, there may be more than one UTF-8 character (international keyboards may do this).
                for (unsigned int i = 0; i < SDL_TEXTINPUTEVENT_TEXT_SIZE;)
                {
                    if (raw.text.text[i] == '\0')
                    {
                        break;
                    }
                    Uint8 bytes = max(1, (int)Utilities::CheckUTF8(raw.text.text[i]));

                    InputChar data;
                    for (unsigned int counti = i + bytes; i < counti; i++)
                    {
                        data.utf8 += raw.text.text[i];
                    }

                    // Convert to codepoint.
                    unicode.push_back(Utilities::GetCodepointUTF8(data.utf8));
                    data.codepoint = unicode.back();

                    result = max(CallAction(data, data.codepoint), result);

                    // Increment to next UTF-8 character.
                    i += bytes;
                }
            }
            // TODO: complex editing events such as CTRL-C copy and CTRL-V paste. Ideally make these inputs bindable too.
            /*else if ()
            {
            }*/
            return result;
        }

        void TextInputHandler::StartListening()
        {
            if (!listenForTextInput)
            {
                if (numTextInputListeners == 0)
                {
                    SDL_StartTextInput();
                }
                numTextInputListeners++;
                listenForTextInput = true;
            }
        }

        void TextInputHandler::StopListening(bool clearText)
        {
            if (listenForTextInput)
            {
                numTextInputListeners = numTextInputListeners > 0 ? numTextInputListeners - 1 : 0;
                listenForTextInput = false;
                if (numTextInputListeners == 0)
                {
                    SDL_StopTextInput();
                    if (clearText)
                    {
                        text.clear();
                        unicode.clear();
                    }
                }
            }
        }

        bool TextInputHandler::IsListening()
        {
            return listenForTextInput;
        }

        void TextInputHandler::Clear()
        {
            text.clear();
            unicode.clear();
        }

        string TextInputHandler::GetText()
        {
            return text;
        }

        vector<Uint32> TextInputHandler::GetUnicode()
        {
            return unicode;
        }

        InputChar TextInputHandler::PopChar()
        {
            InputChar popped;
            if (text.empty())
            {
                return popped;
            }
            popped.codepoint = unicode.back();
            unicode.pop_back();
            for (unsigned int i = 0; i < 4; i++)
            {
                popped.utf8[i] = text.back();
                text.pop_back();
                if (Utilities::CheckUTF8(popped.utf8[i]) != 1)
                {
                    break;
                }
            }
            return popped;
        }

    }

}
