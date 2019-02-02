#include "audio.h"

using namespace std;

namespace ossium
{

    ///
    /// AudioClip
    ///

    AudioClip::AudioClip()
    {
        audioChunk = NULL;
    }

    AudioClip::~AudioClip()
    {
        free();
    }

    void AudioClip::free()
    {
        if (audioChunk != NULL)
        {
            Mix_FreeChunk(audioChunk);
            audioChunk = NULL;
        }
    }

    bool AudioClip::load(string guid_path, int* loadArgs)
    {
        free();
        audioChunk = Mix_LoadWAV(guid_path.c_str());
        if (audioChunk == NULL)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load audio clip '%s'! Mix_Error: %s", guid_path.c_str(), Mix_GetError());
            return false;
        }
        return true;
    }

    bool AudioClip::init()
    {
        return audioChunk != NULL;
    }

    Mix_Chunk* AudioClip::GetChunk()
    {
        return audioChunk;
    }

    ///
    /// ChannelController
    ///

    namespace AudioInternals
    {

        void OnAnyChannelFinished(int id)
        {
            ChannelController::_Instance().ChannelFinished(id);
        }

        ChannelController::ChannelController()
        {
            numChannels = 0;
            counter = 0;
        }

        ChannelController::~ChannelController()
        {
            /// Don't call Clear() as that calls an SDL_Mixer function, and SDL_Mixer is probably all cleaned up already
            /// as this is a singleton
            channels.clear();
            numChannels = 0;
            counter = 0;
        }

        void ChannelController::Clear()
        {
            channels.clear();
            Mix_AllocateChannels(0);
            numChannels = 0;
            counter = 0;
        }

        void ChannelController::Init(int num_channels)
        {
            Clear();
            Mix_AllocateChannels(num_channels);
            for (int i = 0; i < num_channels; i++)
            {
                /// Initialise all channels
                channels.push_back(nullptr);
            }
            numChannels = num_channels;
            Mix_ChannelFinished(*OnAnyChannelFinished);
        }

        int ChannelController::ReserveChannel(AudioSource* callback)
        {
            if (callback != nullptr)
            {
                int originalIndex = counter;
                do
                {
                    counter = wrap(counter, 1, 0, numChannels - 1);
                    if (channels[counter] == nullptr)
                    {
                        channels[counter] = callback;
                        return counter;
                    }
                } while (counter != originalIndex);
                return -1;
            }
            return -1;
        }

        void ChannelController::FreeChannel(int id)
        {
            if (id < 0)
            {
                Init(numChannels);
            }
            else if (id >= numChannels)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "ChannelController: Channel id is out of range.");
                return;
            }
            else
            {
                /// Frees the channel, but does not halt any playback on the channel
                channels[id] = nullptr;
            }
        }

        void ChannelController::ChannelFinished(int id)
        {
            if (id < numChannels && !channels.empty())
            {
                if (channels[id] != nullptr)
                {
                    /// Call the associated callback
                    (channels[id])->OnPlayFinished();
                }
            }
        }

    }

    ///
    /// AudioBus
    ///

    AudioBus::AudioBus()
    {
        linkedBus = nullptr;
        bypass = false;
    }

    void AudioBus::SetName(string setName)
    {
        name = setName;
    }

    string AudioBus::GetName()
    {
        return name;
    }

    void AudioBus::Link(AudioBus* bus)
    {
        Unlink();
        bus->input_buses.insert(this);
        linkedBus = bus;
    }

    void AudioBus::Unlink()
    {
        if (linkedBus != nullptr)
        {
            linkedBus->input_buses.erase(this);
            linkedBus = nullptr;
        }
    }

    AudioBus* AudioBus::GetLinkedBus()
    {
        return linkedBus;
    }

    bool AudioBus::IsLinked()
    {
        return linkedBus != nullptr;
    }

    float AudioBus::GetFinalVolume()
    {
        if (IsLinked())
        {
            return this->GetVolume() * linkedBus->GetFinalVolume();
        }
        return this->GetVolume();
    }

    Sint16 AudioBus::GetFinalPanning()
    {
        if (IsLinked())
        {
            return this->GetPanning() + linkedBus->GetFinalPanning();
        }
        return this->GetPanning();
    }

    void AudioBus::OnVolumeChanged()
    {
        /// Iterate over all input signals and change their volumes accordingly
        for (auto i = input_signals.begin(); i != input_signals.end(); i++)
        {
            (*i)->OnVolumeChanged();
        }
    }

    ///
    /// AudioSource
    ///

    int AudioSource::total_channels = 0;

    AudioInternals::ChannelController& AudioSource::_channelController = AudioInternals::ChannelController::_Instance();

    AudioSource::AudioSource()
    {
        channel_id = -1;
    }

    AudioSource::~AudioSource()
    {
        if (channel_id >= 0)
        {
            _channelController.FreeChannel(channel_id);
            /// Halt the channel now we've freed it because something is probably playing
            Mix_HaltChannel(channel_id);
            Mix_SetPosition(channel_id, 0, 0);
            Mix_Volume(channel_id, 128);
            channel_id = -1;
        }
    }

    void AudioSource::Link(AudioBus* bus)
    {
        Unlink();
        bus->input_signals.insert(this);
        linkedBus = bus;
    }

    void AudioSource::Unlink()
    {
        if (linkedBus != nullptr)
        {
            linkedBus->input_signals.erase(this);
            linkedBus = nullptr;
        }
    }

    void AudioSource::Play(AudioClip* sample, Sint16 panning, float vol, int repeats)
    {
        if (channel_id >= 0)
        {
            /// Something is playing. Stop it and use that channel
            Mix_HaltChannel(channel_id);
        }
        channel_id = _channelController.ReserveChannel(this);
        if (vol < 0.0f)
        {
            SetStereoVolume(GetVolume(), panning);
        }
        else
        {
            SetStereoVolume(vol, panning);
        }
        Mix_PlayChannel(channel_id, sample->GetChunk(), repeats);
    }

    void AudioSource::Play(AudioClip* sample, float vol, int repeats)
    {
        Play(sample, GetPanning(), vol, repeats);
    }

    bool AudioSource::IsPlaying()
    {
        return channel_id >= 0;
    }

    bool AudioSource::IsLinked()
    {
        return linkedBus != nullptr;
    }

    float AudioSource::GetFinalVolume()
    {
        if (IsLinked())
        {
            return this->GetVolume() * linkedBus->GetFinalVolume();
        }
        return this->GetVolume();
    }

    Sint16 AudioSource::GetFinalPanning()
    {
        if (IsLinked())
        {
            return this->GetPanning() + linkedBus->GetFinalPanning();
        }
        return this->GetPanning();
    }

    void AudioSource::OnVolumeChanged()
    {
        if (channel_id >= 0)
        {
            /// This is okay because it's not set as an SDL_Mixer callback
            Mix_Volume(channel_id, (int)mapRange(GetFinalVolume(), 0.0f, 1.0f, 0.0f, 128.0f));
            Mix_SetPosition(channel_id, clamp(GetFinalPanning(), 0, 360), 0);
        }
    }

    void AudioSource::OnPlayFinished()
    {
        /// AudioSource is responsible for freeing the channel as it reserved it in the first place
        _channelController.FreeChannel(channel_id);
        /// Unregister panning effect
        Mix_SetPosition(channel_id, 0, 0);
        /// Reset volume to default
        Mix_Volume(channel_id, 128);
        channel_id = -1;
    }

/*    ///
    /// AudioStream
    ///

    namespace AudioInternals
    {

        AudioStream::AudioStream()
        {
            stream = NULL;
            paused = false;
            started = false;
        }

        AudioStream::~AudioStream()
        {
            Free();
        }

        void AudioStream::Free()
        {
            if (started)
            {
                Stop();
            }
            if (stream != NULL)
            {
                Mix_FreeMusic(stream);
            }
        }

        bool AudioStream::Play(string path, int loops, int ms)
        {
            Free();
            stream = Mix_LoadMUS(path.c_str());
            if (stream != NULL)
            {
                paused = false;
                started = true;
                Mix_PlayMusic(stream, loops);
                cachedPath = path;
                return true;
            }
            return false;
        }

        bool AudioStream::Play(int loops, int ms)
        {
            if (started && stream != NULL)
            {
                Resume();
            }
            else if (cachedPath != "")
            {
                /// Attempt to use the cached path
                Play(cachedPath, loops, ms);
            }
            return false;
        }

        void AudioStream::Pause()
        {
            if (started && !paused)
            {
                paused = true;
                Mix_PauseMusic();
            }
        }

        void AudioStream::Resume()
        {
            if (started && paused)
            {
                paused = false;
                Mix_ResumeMusic();
            }
        }

        void AudioStream::Stop()
        {
            if (started)
            {
                paused = false;
                started = false;
            }
        }

        Mix_Music* AudioStream::_GetStream()
        {
            return stream;
        }

        string AudioStream::GetPath()
        {
            return cachedPath;
        }

    }
*/
}
