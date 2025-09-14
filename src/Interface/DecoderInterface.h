#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/DecoderData.h"
#include "Kernel/CodecDataInfo.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DecoderInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const ThreadMutexInterfacePtr & _mutex ) = 0;
        virtual void finalize() = 0;

    public:
        virtual const ContentInterfacePtr & getContent() const = 0;
        virtual const InputStreamInterfacePtr & getStream() const = 0;

    public:
        virtual void setCodecDataInfo( const CodecDataInfo * _dataInfo ) = 0;
        virtual const CodecDataInfo * getCodecDataInfo() const = 0;

    public:
        virtual bool prepareData( const ContentInterfacePtr & _content, const InputStreamInterfacePtr & _stream ) = 0;

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

