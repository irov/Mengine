#pragma once

#include "Interface/Interface.h"
#include "Interface/CodecInterface.h"
#include "Interface/InputStreamInterface.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct DecoderData
    {
#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
        DecoderData() = default;
        virtual ~DecoderData() = default;
#endif

        void * buffer;
        size_t size;
    };
    //////////////////////////////////////////////////////////////////////////
    class DecoderInterface
        : public Interface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual const InputStreamInterfacePtr & getStream() const = 0;

    public:
        virtual void setCodecDataInfo( const CodecDataInfo * _dataInfo ) = 0;
        virtual const CodecDataInfo * getCodecDataInfo() const = 0;

    public:
        virtual bool prepareData( const InputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual size_t decode( const DecoderData * _data ) = 0;
        virtual bool rewind() = 0;

    public:
        virtual bool seek( float _time ) = 0;
        virtual float tell() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderInterface> DecoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

