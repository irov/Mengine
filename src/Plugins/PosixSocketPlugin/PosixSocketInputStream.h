#pragma once

#include "Interface/InputStreamInterface.h"

#include "PosixSocketProviderInterface.h"

namespace Mengine
{
    class PosixSocketInputStream
        : public InputStreamInterface
        , virtual public PosixSocketProviderInterface
    {
    public:
        PosixSocketInputStream();
        ~PosixSocketInputStream() override;

    public:
        size_t read( void * _buffer, size_t _size ) override;
        bool seek( size_t _pos ) override;
        bool skip( size_t _pos ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t & _time ) const override;

    public:
        bool memory( void ** _memory, size_t * _size ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PosixSocketInputStream, InputStreamInterface> PosixSocketInputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}