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

                    // Compute the Unicode codepoint
                    data.codepoint = Utilities::GetCodepointUTF8(data.utf8);

                    // Insert the UTF-8 character into the input text string.
                    Insert(data.utf8);

                    // It may be useful to use bindless actions, e.g. to check for characters that are not allowed in this text field.
                    result = max(CallAction(data, data.codepoint), result);
                }
            }
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

        void TextInputHandler::Insert(string str)
        {
            if (cursorIndex == unicode.size())
            {
                Uint32 start_text = unicode.empty() ? 0 : unicodeToText[cursorIndex - 1];
                text += str;
                for (unsigned int i = 0, counti = str.length(); i < counti; cursorIndex++)
                {
                    Uint8 bytes = max(1, (int)Utilities::CheckUTF8(str[i]));
                    unicode.push_back(Utilities::GetCodepointUTF8(str.substr(i, bytes)));
                    i += bytes;
                    unicodeToText.push_back(i + start_text);
                }
            }
            else
            {
                Uint32 start_text = unicodeToText[cursorIndex];
                text.insert(start_text, str);
                for (unsigned int i = 0, counti = str.length(); i < counti; cursorIndex++)
                {
                    Uint8 bytes = max(1, (int)Utilities::CheckUTF8(str[i]));
                    unicode.insert(unicode.begin() + cursorIndex, Utilities::GetCodepointUTF8(str.substr(i, bytes)));
                    unicodeToText.insert(unicodeToText.begin() + cursorIndex, i + start_text);
                    i += bytes;
                }
            }
        }

        void TextInputHandler::Erase(Uint32 len)
        {
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
            cursorIndex--;
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

        void TextInputHandler::SetCursorIndex(Uint32 unicode_index)
        {
            cursorIndex = Utilities::Clamp((int)unicode_index, 0, (int)unicode.size());
        }

        Uint32 TextInputHandler::GetCursorIndex()
        {
            return cursorIndex;
        }

    }

}
