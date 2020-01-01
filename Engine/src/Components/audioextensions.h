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
#ifndef AUDIOEXTENSIONS_H
#define AUDIOEXTENSIONS_H

#include "../Core/audio.h"
#include "../Core/component.h"

namespace Ossium
{

    inline namespace Audio
    {

        struct OSSIUM_EDL AudioSourceSchema : public Schema<AudioSourceSchema, 2>
        {
        public:
            DECLARE_BASE_SCHEMA(AudioSourceSchema, 2);

        protected:
            /// Path to the currently playing audio sample.
            M(string, samplePath);

            /// Is the source looping forever?
            M(bool, looping) = false;

        };

        namespace Internal
        {

            class OSSIUM_EDL AudioSourceSchemaCombiner : public AudioPlayer, public AudioSourceSchema
            {
            public:
                CONSTRUCT_SCHEMA(AudioPlayer, AudioSourceSchema);
            };

        }

        class OSSIUM_EDL AudioSource : public Component, public Internal::AudioSourceSchemaCombiner
        {
        public:
            CONSTRUCT_SCHEMA(Component, AudioSourceSchemaCombiner);
            DECLARE_COMPONENT(Component, AudioSource);

            /// Plays an audio sample with panning and volume adjusted to simulate spatial audio for the main listener at a particular position.
            /// Doesn't account for walls and physics objects, purely distance and direction "as the crow flies" from the listener.
            void Play(AudioClip* sample, float vol = -1.0f, int repeats = 0);

            /// Updates the volume and panning of the audio playback for simple spatial effects.
            void Update();

            /// If a sample is specified, play it.
            void OnLoadFinish();

        private:
            /// Sets the spatial audio attenuation and panning of this source.
            void SetAudioPosition();

            using Internal::AudioSourceSchemaCombiner::Play;

        };

        struct OSSIUM_EDL AudioListenerSchema : public Schema<AudioListenerSchema, 1>
        {
            DECLARE_BASE_SCHEMA(AudioListenerSchema, 1);

            /// The minimum distance at which audio sources aren't listened to.
            M(float, cutoff) = 1280.0f;

        };

        class OSSIUM_EDL AudioListener : public Component, public AudioListenerSchema
        {
        public:
            DECLARE_COMPONENT(Component, AudioListener);
            CONSTRUCT_SCHEMA(Component, AudioListenerSchema);

            friend class AudioSource;

            /// Sets the static mainListener instance to this instance if not already set.
            /// Logs a warning if the main listener is already set.
            void OnCreate();

            /// If this is the main listener, set the main listener pointer to nullptr.
            void OnDestroy();

        private:
            static AudioListener* mainListener;

        };

    }

}

#endif // AUDIOEXTENSIONS_H
