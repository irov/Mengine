#pragma once

#include "Interface/UnknownInterface.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace Mengine
{
    class AndroidSoundSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual SLEngineItf getEngineInterface() const = 0;
        virtual SLObjectItf getOutputMixObject() const = 0;
    };
}
