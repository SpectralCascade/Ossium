#include "textinput.h"
#include "utf8.h"

using namespace std;

namespace Ossium
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

    void TextInputHandler::SetText(string str)
    {
        text.clear();
        unicode.clear();
        unicodeToText.clear();
        Uint32 oldIndex = cursorIndex;
        cursorIndex = 0;
        Insert(str);
        cursorIndex = min((Uint32)unicode.size(), oldIndex);
    }

    void TextInputHandler::Clear()
    {
        text.clear();
        unicode.clear();
        unicodeToText.clear();
        cursorIndex = 0;
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
            Uint32 start_text = unicodeToText.empty() || cursorIndex == 0 ? 0 : (unicodeToText[cursorIndex - 1] + (Uint32)max(1, (int)Utilities::CheckUTF8(text[unicodeToText[cursorIndex - 1]])));
            text += str;
            for (Uint32 i = 0, counti = str.length(); i < counti; cursorIndex++)
            {
                Uint8 bytes = max(1, (int)Utilities::CheckUTF8(str[i]));
                unicode.push_back(Utilities::GetCodepointUTF8(str.substr(i, bytes)));
                unicodeToText.push_back(i + start_text);
                //Log.Info("Inserted {0} byte(s) for character {1} [Unicode: {2}] at string index {3} (unicode index {4})", (int)bytes, str.substr(i, bytes), unicode.back(), unicodeToText.back(), unicode.size() - 1);
                i += bytes;
            }
        }
        else
        {
            Uint32 start_text = unicodeToText.empty() ? 0 : unicodeToText[cursorIndex];
            text.insert(start_text, str);
            Uint32 index = 0;
            for (Uint32 counti = str.length(); index < counti; cursorIndex++)
            {
                Uint8 bytes = max(1, (int)Utilities::CheckUTF8(str[index]));
                unicode.insert(unicode.begin() + cursorIndex, Utilities::GetCodepointUTF8(str.substr(index, bytes)));
                unicodeToText.insert(unicodeToText.begin() + cursorIndex, index + start_text);
                //Log.Info("Inserted {0} byte(s) for character {1} [Unicode: {2}] at string index {3} (unicode index {4})", (int)bytes, str.substr(index, bytes), unicode[cursorIndex], unicodeToText[cursorIndex], cursorIndex);
                index += bytes;
            }
            for (Uint32 i = cursorIndex; i < unicodeToText.size(); i++)
            {
                // Update the mapping
                unicodeToText[i] += index;
            }
        }
    }

    void TextInputHandler::Erase(Uint32 len)
    {
        if (cursorIndex < unicode.size() && len > 0)
        {
            Uint32 strLen = (cursorIndex + len >= unicode.size() ?
                unicodeToText[cursorIndex + len - 1] + (Uint32)max(1, (int)Utilities::CheckUTF8(unicodeToText[cursorIndex + len - 1])) :
                unicodeToText[cursorIndex + len]) - unicodeToText[cursorIndex];

            //Log.Info("Erasing string of length {0} at index {1}", strLen, cursorIndex);

            text.erase(unicodeToText[cursorIndex], strLen);

            unicode.erase(unicode.begin() + cursorIndex, unicode.begin() + cursorIndex + len);
            unicodeToText.erase(unicodeToText.begin() + cursorIndex, unicodeToText.begin() + cursorIndex + len);
            for (Uint32 i = cursorIndex; i < unicodeToText.size(); i++)
            {
                // Update the mapping
                unicodeToText[i] -= strLen;
            }
        }
    }

    InputChar TextInputHandler::PopChar()
    {
        InputChar popped;
        if (unicode.empty() || cursorIndex <= 0)
        {
            return popped;
        }
        cursorIndex--;
        popped.codepoint = unicode[cursorIndex];
        popped.utf8 = text.substr(unicodeToText[cursorIndex], max(1, (int)Utilities::CheckUTF8(text[unicodeToText[cursorIndex]])));
        Erase(1);
        return popped;
    }

    void TextInputHandler::SetCursorIndex(Uint32 unicode_index)
    {
        cursorIndex = min(unicode_index, (Uint32)unicode.size());
    }

    Uint32 TextInputHandler::GetCursorIndex()
    {
        return cursorIndex;
    }

}
