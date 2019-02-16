#pragma once

#include "Interface/OutputStreamInterface.h"

#include "PosixSocketProviderInterface.h"

namespace Mengine
{
    class PosixSocketOutputStream
        : public OutputStreamInterface
        , virtual public PosixSocketProviderInterface
    {
    public:
        PosixSocketOutputStream();
        ~PosixSocketOutputStream() override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PosixSocketOutputStream, OutputStreamInterface> PosixSocketOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}