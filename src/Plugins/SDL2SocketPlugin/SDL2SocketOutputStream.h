#pragma once

#include "Interface/OutputStreamInterface.h"

#include "SDLSocketProviderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLSocketOutputStream
        : public OutputStreamInterface
        , virtual public SDLSocketProviderInterface
    {
    public:
        SDLSocketOutputStream();
        ~SDLSocketOutputStream() override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;

    public:
        bool flush() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLSocketOutputStream, OutputStreamInterface> SDLSocketOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}