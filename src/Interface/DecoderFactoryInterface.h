#pragma once

#include "Interface/CodecFactoryInterface.h"
#include "Interface/DecoderInterface.h"
#include "Interface/DocumentInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DecoderFactoryInterface
        : public CodecFactoryInterface
    {
    public:
        virtual DecoderInterfacePtr createDecoder( const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderFactoryInterface> DecoderFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

