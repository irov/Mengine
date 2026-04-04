#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/SoundSourceInterface.h"

namespace Mengine
{
    class AppleSoundSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual bool acquireSourceBus( const SoundSourceInterfacePtr & _source, uint32_t _frequency, uint32_t _channels, float _gain, uint32_t * const _busIndex ) = 0;
        virtual void releaseSourceBus( uint32_t _busIndex ) = 0;
        virtual bool setSourceBusVolume( uint32_t _busIndex, float _gain ) = 0;
        virtual bool enableSourceBus( uint32_t _busIndex, bool _enable ) = 0;
    };
}