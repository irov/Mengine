#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/OutputStreamInterface.h"

#include "Kernel/EncoderData.h"
#include "Kernel/CodecDataInfo.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EncoderInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const OutputStreamInterfacePtr & _stream ) = 0;
        virtual void finalize() = 0;

    public:
        virtual size_t encode( const EncoderData * _encoderData, const CodecDataInfo * _dataInfo ) = 0;

    public:
        virtual OutputStreamInterfacePtr getStream() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EncoderInterface> EncoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

