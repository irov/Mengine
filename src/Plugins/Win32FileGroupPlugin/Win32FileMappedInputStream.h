#pragma once

#include "Interface/InputStreamInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ThreadGuard.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32FileMappedInputStream
        : public InputStreamInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Win32FileMappedInputStream );

    public:
        Win32FileMappedInputStream();
        ~Win32FileMappedInputStream() override;

    public:
        bool mapViewOfFile( HANDLE _hMapping, DWORD _dwAllocationGranularity, size_t _offset, size_t _size );
        void unmap();

    public:
        size_t read( void * const _buffer, size_t _size ) override;
        bool seek( size_t _pos ) override;
        bool rseek( size_t _pos ) override;
        bool skip( size_t _pos ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t * const _time ) const override;

    public:
        bool memory( void ** const _memory, size_t * const _size ) override;

    protected:
        LPVOID m_memoryGranularity;

        size_t m_size;

        uint8_t * m_base;
        uint8_t * m_pos;
        uint8_t * m_end;

        MENGINE_THREAD_GUARD_INIT( Win32FileMappedInputStream );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32FileMappedInputStream, InputStreamInterface> Win32FileMappedInputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
