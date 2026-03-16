#pragma once

#include "Interface/SoundBufferInterface.h"
#include "Interface/SoundCodecInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/MemoryInterfacePtr.h"

namespace Mengine
{
    class IOSAVSoundSystem;

    class IOSAVSoundBuffer
        : public SoundBufferInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( IOSAVSoundBuffer );

    public:
        IOSAVSoundBuffer();
        ~IOSAVSoundBuffer() override;

    public:
        void initialize( IOSAVSoundSystem * _soundSystem, const SoundDecoderInterfacePtr & _decoder, bool _streamable );

    public:
        bool acquireSoundBuffer() override;
        void releaseSoundBuffer() override;
        bool updateSoundBuffer() override;
        const SoundDecoderInterfacePtr & getDecoder() const override;

    public:
        const SoundCodecDataInfo & getDataInfo() const;
        const MemoryInterfacePtr & getMemory() const;
        bool isStreamable() const;

    public:
        void * createNativePCMBuffer( float _startPositionMs ) const;

    protected:
        IOSAVSoundSystem * m_soundSystem;
        SoundDecoderInterfacePtr m_soundDecoder;

        MemoryInterfacePtr m_memory;
        SoundCodecDataInfo m_dataInfo;

        bool m_streamable;
    };

    typedef IntrusivePtr<IOSAVSoundBuffer, SoundBufferInterface> IOSAVSoundBufferPtr;
}

