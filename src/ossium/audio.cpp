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
    /// Audio Channel
    ///

    void AudioChannel::SetName(string setName)
    {
        name = setName;
    }

    string AudioChannel::GetName()
    {
        return name;
    }

    void AudioChannel::OnVolumeChanged()
    {
        /// Iterate over inputs and change their volumes accordingly
        for (auto i = inputs.begin(); i != inputs.end(); i++)
        {
            (*i)->derived()->OnVolumeChanged();
        }
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

        void ChannelController::Clear()
        {
            Mix_AllocateChannels(0);
            channels.clear();
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
                Mix_HaltChannel(id);
                channels[id] = nullptr;
            }
        }

        void ChannelController::ChannelFinished(int id)
        {
            if (id < numChannels)
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
    /// AudioSource
    ///

    int AudioSource::total_channels = 0;

    AudioInternals::ChannelController& AudioSource::_channelController = AudioInternals::ChannelController::_Instance();

    AudioSource::AudioSource()
    {
        channel_id = -1;
        linked_channel = nullptr;
    }

    AudioSource::~AudioSource()
    {
        if (channel_id >= 0)
        {
            _channelController.FreeChannel(channel_id);
            channel_id = -1;
        }
    }

    void AudioSource::Play(AudioClip* sample, float vol, int repeats)
    {
        if (channel_id >= 0)
        {
            /// Something is playing. Stop it and use that channel
            Mix_HaltChannel(channel_id);
        }
        channel_id = _channelController.ReserveChannel(this);
        SetVolume(vol);
        Mix_PlayChannel(channel_id, sample->GetChunk(), repeats);
    }

    void AudioSource::OnVolumeChanged()
    {
        if (channel_id >= 0)
        {
            Mix_Volume(channel_id, (int)mapRange(GetFinalVolume(), 0.0f, 1.0f, 0.0f, 128.0f));
        }
    }

    void AudioSource::OnPlayFinished()
    {
        _channelController.FreeChannel(channel_id);
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
