#pragma once

#include "Config/Typedef.h"

#include "Interface/CodecFactoryInterface.h"
#include "Interface/EncoderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EncoderFactoryInterface
        : public CodecFactoryInterface
    {
    public:
        virtual EncoderInterfacePtr createEncoder( const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EncoderFactoryInterface> EncoderFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

