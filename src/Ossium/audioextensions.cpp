#include "transform.h"
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
        /// Store state
        looping = repeats < 0;
        samplePath = looping ? sample->GetPath() : "";

        /// Now set positioning and play the sample
        SetAudioPosition();
        AudioPlayer::Play(sample, vol, repeats);
    }

    void AudioSource::Update()
    {
        SetAudioPosition();
    }

    void AudioSource::SetAudioPosition()
    {
        /// Calculate difference as a vector from the listener to this source.
        Vector2 difference = AudioListener::mainListener != nullptr ? WorldPosition() - AudioListener::mainListener->WorldPosition() : Vector2(0, 0);
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
                        mainListener->entity->GetName(),
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
