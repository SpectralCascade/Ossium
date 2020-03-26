#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include "input.h"

namespace Ossium
{

    inline namespace Input
    {

        struct OSSIUM_EDL InputChar
        {
            string utf8;

            Uint32 codepoint = 0;

        };

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

            /// Returns text collected during input handling.
            string GetText();

            /// Returns the Unicode codepoints of the input text string.
            vector<Uint32> GetUnicode();

            /// Removes and returns the last character that was input. Returns an empty string if the buffer is empty.
            InputChar PopChar();

            /// Returns true when listening to text input events.
            bool IsListening();

        protected:
            /// The raw text input buffer.
            string text;

            /// The Unicode string of codepoints.
            vector<Uint32> unicode;

        private:
            /// Whether this text input instance is listening for text input events or not.
            bool listenForTextInput = false;

            /// Keeps count of TextInput instances that are listening.
            static unsigned int numTextInputListeners;

        };

    }

}

#endif // TEXTINPUT_H
