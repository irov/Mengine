#pragma once

#include "Interface/InputStreamInterface.h"

#include "SDLSocketProviderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLSocketInputStream
        : public InputStreamInterface
        , virtual public SDLSocketProviderInterface
    {
    public:
        SDLSocketInputStream();
        ~SDLSocketInputStream() override;

    public:
        size_t read( void * const _buffer, size_t _size ) override;
        bool seek( size_t _pos ) override;
        void rewind() override;
        bool rseek( size_t _pos ) override;
        bool skip( size_t _pos ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t * const _time ) const override;

    public:
        bool memory( void ** const _memory, size_t * const _size ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLSocketInputStream, InputStreamInterface> SDLSocketInputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}