#pragma once

#include "Interface/UnknownInterface.h"

#include "OpenALSoundIncluder.h"

namespace Mengine
{
    class OpenALSoundSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual ALuint genSourceId() = 0;
        virtual void releaseSourceId( ALuint _sourceId ) = 0;

        virtual ALuint genBufferId() = 0;
        virtual void releaseBufferId( ALuint _bufferId ) = 0;
    };
}