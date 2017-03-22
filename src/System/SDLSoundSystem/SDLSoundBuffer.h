#	pragma once

#	include "Interface/SoundSystemInterface.h"
#	include "Interface/SoundCodecInterface.h"

#   include "Core/ServantBase.h"

#	include "SDL.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class SDLSoundBuffer
        : public ServantBase<SoundBufferInterface>
    {
    public:
        SDLSoundBuffer();
        ~SDLSoundBuffer();
       
    public:
        const SoundDecoderInterfacePtr & getDecoder() const override;

    public:
        virtual bool load( const SoundDecoderInterfacePtr & _soundDecoder );
        virtual void play( int _channel, bool _loop );

    public:
        virtual bool update() override;
        
    protected:
        ServiceProviderInterface * m_serviceProvider;
        SoundDecoderInterfacePtr m_soundDecoder;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<SDLSoundBuffer> SDLSoundBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}   // namespace Menge
