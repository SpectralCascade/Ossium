#include "audio.h"

using namespace std;

namespace Ossium
{

    inline namespace Audio
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
            Free();
        }

        void AudioClip::Free()
        {
            if (audioChunk != NULL)
            {
                Mix_FreeChunk(audioChunk);
                audioChunk = NULL;
            }
        }

        bool AudioClip::Load(string guid_path, int* loadArgs)
        {
            Free();
            audioChunk = Mix_LoadWAV(guid_path.c_str());
            if (audioChunk == NULL)
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load audio clip '%s'! Mix_Error: %s", guid_path.c_str(), Mix_GetError());
                return false;
            }
            return true;
        }

        bool AudioClip::Init()
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

        namespace Internals
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
            muted = false;
            input_stream = nullptr;
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
            if (!bypass && muted)
            {
                return 0;
            }
            if (IsLinked())
            {
                if (bypass)
                {
                    /// Bypass the settings in this bus
                    return linkedBus->GetFinalVolume();
                }
                return this->GetVolume() * linkedBus->GetFinalVolume();
            }
            return this->GetVolume();
        }

        Sint16 AudioBus::GetFinalPanning()
        {
            if (IsLinked())
            {
                if (bypass)
                {
                    /// Bypass the settings in this bus
                    return linkedBus->GetFinalPanning();
                }
                return this->GetPanning() + linkedBus->GetFinalPanning();
            }
            return this->GetPanning();
        }

        void AudioBus::SetBypassMode(bool bypass_channel)
        {
            if (bypass_channel != bypass)
            {
                bypass = bypass_channel;
                /// Trigger a recalculation of volume level
                OnVolumeChanged();
            }
        }

        bool AudioBus::IsBypassed()
        {
            return bypass;
        }

        bool AudioBus::IsMuted()
        {
            return muted;
        }

        void AudioBus::Mute()
        {
            if (!muted)
            {
                muted = true;
                OnVolumeChanged();
            }
        }

        void AudioBus::Unmute()
        {
            if (muted)
            {
                muted = false;
                OnVolumeChanged();
            }
        }

        void AudioBus::OnVolumeChanged()
        {
            /// Iterate over all input buses and signals and change their volumes accordingly
            for (auto i = input_buses.begin(); i != input_buses.end(); i++)
            {
                (*i)->OnVolumeChanged();
            }
            for (auto i = input_signals.begin(); i != input_signals.end(); i++)
            {
                (*i)->OnVolumeChanged();
            }
            if (input_stream != nullptr)
            {
                input_stream->OnVolumeChanged();
            }
        }

        ///
        /// AudioSource
        ///

        int AudioSource::total_channels = 0;

        Internals::ChannelController& AudioSource::_channelController = Internals::ChannelController::_Instance();

        AudioSource::AudioSource()
        {
            linkedBus = nullptr;
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
            if (Mix_Paused(channel_id))
            {
                Mix_Resume(channel_id);
            }
            paused = false;
        }

        void AudioSource::Play(AudioClip* sample, float vol, int repeats)
        {
            Play(sample, GetPanning(), vol, repeats);
        }

        bool AudioSource::IsPlaying()
        {
            return channel_id >= 0;
        }

        void AudioSource::Pause()
        {
            if (IsPlaying())
            {
                Mix_Pause(channel_id);
                paused = true;
            }
        }

        void AudioSource::Resume()
        {
            if (IsPaused() && IsPlaying())
            {
                Mix_Resume(channel_id);
                paused = false;
            }
        }

        bool AudioSource::IsPaused()
        {
            return paused;
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
                Mix_SetPosition(channel_id, wrap(0, GetFinalPanning(), 0, 360), 0);
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

        ///
        /// AudioStream
        ///

        namespace Internals
        {

            void MusicFinished()
            {
                SoundStream.Init();
            }

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

            void AudioStream::Init()
            {
                paused = false;
                started = false;
            }

            void AudioStream::Free()
            {
                Stop();
                if (stream != NULL)
                {
                    Mix_FreeMusic(stream);
                    stream = NULL;
                }
            }

            bool AudioStream::Load(string path)
            {
                if (!started)
                {
                    Mix_HookMusicFinished(*MusicFinished);
                }
                Free();
                stream = Mix_LoadMUS(path.c_str());
                if (stream != NULL)
                {
                    cachedPath = path;
                    return true;
                }
                return false;
            }

            void AudioStream::Play(float vol, int loops)
            {
                Stop();
                if (stream != NULL)
                {
                    SetVolume(vol);
                    Mix_VolumeMusic(mapRange(GetFinalVolume(), 0.0f, 1.0f, 0.0f, 128.0f));
                    Mix_PlayMusic(stream, loops);
                    started = true;
                }
                else if (cachedPath != "")
                {
                    /// Attempt to use the cached path
                    if (Load(cachedPath))
                    {
                        SetVolume(vol);
                        Mix_VolumeMusic(mapRange(GetFinalVolume(), 0.0f, 1.0f, 0.0f, 128.0f));
                        Mix_PlayMusic(stream, loops);
                        started = true;
                    }
                }
            }

            void AudioStream::Play(string path, float vol, int loops)
            {
                if (Load(path))
                {
                    cachedPath = path;
                    Play(vol, loops);
                }
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

            bool AudioStream::IsPaused()
            {
                return paused;
            }

            bool AudioStream::IsStarted()
            {
                return started;
            }

            void AudioStream::Stop()
            {
                if (started && Mix_PlayingMusic())
                {
                    Mix_HaltMusic();
                }
                paused = false;
                started = false;
            }

            void AudioStream::Link(AudioBus* bus)
            {
                Unlink();
                bus->input_stream = this;
                linkedBus = bus;
            }

            void AudioStream::Unlink()
            {
                if (IsLinked())
                {
                    linkedBus->input_stream = nullptr;
                    linkedBus = nullptr;
                }
            }

            bool AudioStream::IsLinked()
            {
                return linkedBus != nullptr;
            }

            Mix_Music* AudioStream::GetStream()
            {
                return stream;
            }

            string AudioStream::GetPath()
            {
                return cachedPath;
            }

            const Sint16 AudioStream::GetFinalPanning()
            {
                return 0;
            }

            float AudioStream::GetFinalVolume()
            {
                if (IsLinked())
                {
                    return GetVolume() * linkedBus->GetFinalVolume();
                }
                return GetVolume();
            }

            void AudioStream::OnVolumeChanged()
            {
                Mix_VolumeMusic((int)mapRange(GetFinalVolume(), 0.0f, 1.0f, 0.0f, 128.0f));
            }

        }

    }

    inline namespace global
    {

        Audio::Internals::AudioStream& SoundStream = Audio::Internals::AudioStream::_Instance();

    }

}
