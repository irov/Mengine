#   pragma once

#   include "Interface/SoundSystemInterface.h"

#   include "Core/ServiceBase.h"

#   include "SDLSoundFilter.h"

#   include "Factory/Factory.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
#   ifndef MENGINE_SDL_SOUND_MAX_COUNT
#   define MENGINE_SDL_SOUND_MAX_COUNT 32
#   endif
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t INVALID_SOUND_ID = (uint32_t)-1;
    //////////////////////////////////////////////////////////////////////////
    class SDLSoundSystem
        : public ServiceBase<SoundSystemInterface>
    {
    public:
        SDLSoundSystem();
        ~SDLSoundSystem();

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        void update() override;

    public:
        bool isSilent() const override;
        
    public:
        void onTurnSound( bool _turn ) override;

    public:
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream ) override;

    public:
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer ) override;

    public:
        int findFreeChannel();
        void freeChannel(int channel);

    protected:
        FactoryPtr m_poolSoundBuffer;
        FactoryPtr m_poolSoundBufferStream;
        FactoryPtr m_poolSoundSource;

        bool m_freeChannels[MENGINE_SDL_SOUND_MAX_COUNT];
    };
}	// namespace Menge
