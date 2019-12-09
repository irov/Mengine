#pragma once

#include "Interface/CodecFactoryInterface.h"
#include "Interface/DecoderInterface.h"

#include "Kernel/Document.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DecoderFactoryInterface
        : public CodecFactoryInterface
    {
    public:
        virtual DecoderInterfacePtr createDecoder( const DocumentPtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderFactoryInterface> DecoderFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

