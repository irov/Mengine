#pragma once

#include "Config/Typedef.h"

#include "Interface/Interface.h"
#include "Interface/CodecInterface.h"
#include "Interface/OutputStreamInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct EncoderData
    {
#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
        EncoderData() = default;
        virtual ~EncoderData() = default;
#endif

        const void * buffer;
        size_t size;
    };
    //////////////////////////////////////////////////////////////////////////
    class EncoderInterface
        : public Interface
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

