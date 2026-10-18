#pragma once

#include "Interface/SoundSourceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/SpinLock.h"

#include "Config/Atomic.h"

#include <aaudio/AAudio.h>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ANDROID_MIXER_INPUT_BUS_COUNT
#define MENGINE_ANDROID_MIXER_INPUT_BUS_COUNT (32)
#endif
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const uint32_t AAUDIO_MIXER_OUTPUT_CHANNEL_COUNT = 2;
    //////////////////////////////////////////////////////////////////////////
    typedef Atomic<AAudioStream *> AtomicAAudioStream;
    //////////////////////////////////////////////////////////////////////////
    class AAudioSoundMixer
        : public Factorable
    {
        DECLARE_FACTORABLE( AAudioSoundMixer );

    public:
        AAudioSoundMixer();
        ~AAudioSoundMixer() override;

    public:
        void setStream( AAudioStream * _stream );

    public:
        aaudio_result_t takeError();
        void clearSources();

        bool acquireSourceBus( const SoundSourceInterfacePtr & _source, float _gain, uint32_t * const _busIndex );
        void releaseSourceBus( uint32_t _busIndex );
        bool setSourceBusVolume( uint32_t _busIndex, float _gain );
        bool enableSourceBus( uint32_t _busIndex, bool _enable );

    public:
        aaudio_data_callback_result_t onAAudioSoundMixerAudioReady( AAudioStream * _stream, void * _audioData, int32_t _frames );
        void onAAudioSoundMixerAudioError( AAudioStream * _stream, aaudio_result_t _error );

    protected:
        SoundSourceInterfacePtr detachSourceBus_( uint32_t _busIndex );

    protected:
        SpinLock m_busLock;

        struct AAudioSoundMixerBusDesc
        {
            SpinLock lock;
            SoundSourceInterfacePtr source;
            AtomicFloat gain{0.f};
            AtomicBool enabled{false};
            bool busy = false;
        };
        AAudioSoundMixerBusDesc m_buses[MENGINE_ANDROID_MIXER_INPUT_BUS_COUNT];
        AtomicAAudioStream m_stream;
        aaudio_result_t m_error;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AAudioSoundMixer> AAudioSoundMixerPtr;
    //////////////////////////////////////////////////////////////////////////
}
