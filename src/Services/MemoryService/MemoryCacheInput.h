#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/ThreadGuard.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MemoryService;
    //////////////////////////////////////////////////////////////////////////
    class MemoryCacheInput
        : public MemoryCacheInputInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Document );

    public:
        MemoryCacheInput();
        ~MemoryCacheInput() override;

    public:
        void setMemoryManager( MemoryService * _memoryManager );

    public:
        Pointer cacheBuffer( size_t _size ) override;

    public:
        Pointer getBuffer() const override;
        size_t getSize() const override;

    public:
        size_t read( void * const _buf, size_t _size ) override;
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
        void uncache_();

    protected:
        MemoryService * m_memoryManager;

        UniqueId m_bufferId;

        uint8_t * m_data;
        size_t m_size;

        uint8_t * m_pos;
        uint8_t * m_end;

        MENGINE_THREAD_GUARD_INIT( MemoryCacheInput );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryCacheInput> MemoryCacheInputPtr;
    //////////////////////////////////////////////////////////////////////////
}