#pragma once

#include "Interface/OutputStreamInterface.h"

#include "Win32SocketProviderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32SocketOutputStream
        : public OutputStreamInterface
        , virtual public Win32SocketProviderInterface
    {
    public:
        Win32SocketOutputStream();
        ~Win32SocketOutputStream() override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32SocketOutputStream, OutputStreamInterface> Win32SocketOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}