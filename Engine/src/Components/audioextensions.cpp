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
#include "../Core/transform.h"
#include "audioextensions.h"

namespace Ossium
{

    ///
    /// AudioSource
    ///

    REGISTER_COMPONENT(AudioSource);

    void AudioSource::OnLoadFinish()
    {
        if (looping && !samplePath.empty())
        {
            Play(GetService<ResourceController>()->Get<AudioClip>(samplePath), volume, -1);
        }
        else
        {
            samplePath = "";
        }
    }

    void AudioSource::Play(AudioClip* sample, float vol, int repeats)
    {
        if (sample != nullptr)
        {
            /// Store state
            looping = repeats < 0;
            samplePath = looping ? sample->GetPath() : "";

            /// Now set positioning and play the sample
            SetAudioPosition();
            AudioPlayer::Play(sample, vol, repeats);
        }
        else
        {
            Logger::EngineLog().Error("Cannot play NULL audio sample!");
        }
    }

    void AudioSource::Update()
    {
        SetAudioPosition();
    }

    void AudioSource::SetAudioPosition()
    {
        /// Calculate difference as a vector from the listener to this source.
        Vector2 difference = AudioListener::mainListener != nullptr ? GetTransform()->GetWorldPosition() - AudioListener::mainListener->GetTransform()->GetWorldPosition() : Vector2(0, 0);
        float cutoffSquared = AudioListener::mainListener != nullptr ? AudioListener::mainListener->cutoff * AudioListener::mainListener->cutoff : 255;

        /// Set attenuation based on distance
        /// Use maximum attenuation if distance > cutoff, else calculate based on cutoff.
        float squareDistance = difference.LengthSquared();
        spatialAttenuation = squareDistance > cutoffSquared ? 255 : (Uint8)Utilities::MapRange(squareDistance, 0, cutoffSquared, 0, 255);

        /// Note: calling this method automatically applies spatial attenuation as it calls OnVolumeChanged().
        SetPanning(difference.Rotation());
    }

    ///
    /// AudioListener
    ///

    REGISTER_COMPONENT(AudioListener);

    void AudioListener::OnCreate()
    {
        if (mainListener == nullptr)
        {
            mainListener = this;
        }
        else
        {
            Logger::EngineLog().Warning(
                        "There is more than one audio listener instance active, main listener already set! Main listener entity name is: {0}, id is: {1}",
                        mainListener->entity->name,
                        mainListener->entity->GetID()
            );
        }
    }

    void AudioListener::OnDestroy()
    {
        if (mainListener == this)
        {
            mainListener = nullptr;
        }
    }

    AudioListener* AudioListener::mainListener = nullptr;

}
