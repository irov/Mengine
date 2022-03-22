#pragma once

#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

#include "Kernel/ThreadGuardScope.h"

#ifdef MENGINE_DEBUG
#include "Kernel/DocumentHelper.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MemoryProxyInput
        : public MemoryProxyInputInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MemoryProxyInput );

    public:
        MemoryProxyInput();
        ~MemoryProxyInput() override;

    public:
        Pointer setBuffer( void * _memory, size_t _offset, size_t _size ) override;

    public:
        Pointer getBuffer() const override;
        size_t getSize() const override;

    public:
        size_t read( void * const _buf, size_t _count ) override;
        bool seek( size_t _pos ) override;
        void rewind() override;
        bool rseek( size_t _pos ) override;
        bool skip( size_t _size ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t * const _time ) const override;

    public:
        bool memory( void ** const _memory, size_t * const _size ) override;

    protected:
        uint8_t * m_data;
        size_t m_size;

        uint8_t * m_pos;
        uint8_t * m_end;

        MENGINE_THREAD_GUARD_INIT( MemoryProxyInput );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryProxyInput> MemoryProxyInputPtr;
    //////////////////////////////////////////////////////////////////////////
}