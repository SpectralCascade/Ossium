#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include "input.h"

namespace Ossium
{

    struct OSSIUM_EDL InputChar
    {
        string utf8;

        Uint32 codepoint = 0;

    };

    /// Handles text input events.
    class OSSIUM_EDL TextInputHandler : public InputHandler<TextInputHandler, InputChar, Uint32>
    {
    public:
        /// Handling method
        ActionOutcome HandleInput(const SDL_Event& raw);

        /// Starts listening to text input events.
        void StartListening();

        /// Stops listening to text input events.
        void StopListening(bool clearText = false);

        /// Clears the text input buffer.
        void Clear();

        /// Sets the current text input buffer string and clamps the cursor.
        /// This is useful if you have multiple text input fields but only one TextInput instance to go round.
        /// TODO: this is horrifically inefficient as it means the computed data is thrown away. Instead, make InputTextHandler a specialised type (not bound to input system),
        /// then there can be multiple instances.
        void SetText(string str);

        /// Returns text collected during input handling.
        string GetText();

        /// Inserts some text into the input buffer at the current cursor index.
        void Insert(string str);

        /// Removes part of the text from the current cursor index.
        void Erase(Uint32 len = 1);

        /// Returns the Unicode codepoints of the input text string.
        vector<Uint32> GetUnicode();

        /// Removes and returns the last character that was input. Returns an empty string if the buffer is empty.
        InputChar PopChar();

        /// Returns true when listening to text input events.
        bool IsListening();

        /// Sets the input cursor codepoint index. Note that this is clamped between 0 and the size of GetUnicode() (inclusive-inclusive).
        void SetCursorIndex(Uint32 unicode_index);

        /// Returns the input cursor codepoint index.
        Uint32 GetCursorIndex();

    protected:
        /// Returns the corresponding text index for a given Unicode index.
        Uint32 GetTextIndex(Uint32 unicode_index);

        /// The raw text input buffer.
        string text;

        /// The Unicode string of codepoints.
        vector<Uint32> unicode;

        /// Mapping between unicode indices and the raw text string indices.
        vector<Uint32> unicodeToText;

        /// The current cursor index.
        Uint32 cursorIndex = 0;

    private:
        /// Whether this text input instance is listening for text input events or not.
        bool listenForTextInput = false;

        /// Keeps count of TextInput instances that are listening.
        static unsigned int numTextInputListeners;

    };

}

#endif // TEXTINPUT_H
