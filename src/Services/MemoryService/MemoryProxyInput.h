#pragma once

#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

#include "stdex/thread_guard.h"

#ifdef MENGINE_DEBUG
#include "Kernel/String.h"
#endif

namespace Mengine
{
    class MemoryProxyInput
        : public MemoryProxyInputInterface
        , public Factorable
    {
    public:
        MemoryProxyInput();
        ~MemoryProxyInput() override;

    public:
        Pointer setBuffer( void * _memory, size_t _offset, size_t _size, const Char * _doc ) override;

    public:
        Pointer getBuffer() const override;
        size_t getSize() const override;

    public:
        size_t read( void * _buf, size_t _count ) override;
        bool seek( size_t _pos ) override;
        bool skip( size_t _pos ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t & _time ) const override;

    public:
        bool memory( void ** _memory, size_t * _size ) override;

    protected:
        uint8_t * m_data;
        size_t m_size;

        uint8_t * m_pos;
        uint8_t * m_end;

#ifdef MENGINE_DEBUG
        String m_doc;
#endif

        STDEX_THREAD_GUARD_INIT;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryProxyInput> MemoryProxyInputPtr;
    //////////////////////////////////////////////////////////////////////////
}


