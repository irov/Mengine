#pragma once

#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ThreadGuardScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MemoryInput
        : public MemoryInputInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MemoryInput );

    public:
        MemoryInput();
        ~MemoryInput() override;

    public:
        Pointer newBuffer( size_t _size ) override;

    public:
        Pointer getBuffer() const override;
        size_t getSize() const override;

    public:
        size_t read( void * const _buf, size_t _count ) override;
        bool seek( size_t _carriage ) override;
        void rewind() override;
        bool rseek( size_t _carriage ) override;
        bool skip( size_t _offset ) override;
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

        uint8_t * m_carriage;
        uint8_t * m_end;

        MENGINE_THREAD_GUARD_INIT( MemoryInput );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryInput> MemoryInputPtr;
    //////////////////////////////////////////////////////////////////////////
}


