#   pragma once

#   include "SDLSoundBuffer.h"

#   include "SDL_mixer.h"

namespace Menge
{
    class SDLSoundBufferMemory
        : public SDLSoundBuffer
    {
    public:
        SDLSoundBufferMemory();
        ~SDLSoundBufferMemory();

    public:
        bool load(const SoundDecoderInterfacePtr & _soundDecoder) override;
        void play(int _channel, bool _loop) override;

    protected:
        uint32_t    m_frequency;
        uint32_t    m_channels;
        float       m_length;
        Mix_Chunk   m_chunk;
    };
}   // namespace Menge
