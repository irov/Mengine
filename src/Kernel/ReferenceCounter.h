#pragma once

#include "Kernel/ThreadGuard.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ReferenceCounter
    {
    public:
        ReferenceCounter();
        explicit ReferenceCounter( uint32_t _counter );

    public:
        ~ReferenceCounter();

    public:
        MENGINE_INLINE uint32_t getReferenceCount() const;
        MENGINE_INLINE bool isReferencing() const;

    public:
        bool incref();
        bool decref();

    public:
        bool increfWithMutex();
        bool decrefWithMutex();

    public:
        void reset();

    private:
        uint32_t m_counter;

        MENGINE_THREAD_GUARD_INIT( CompilableReference );
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t ReferenceCounter::getReferenceCount() const
    {
        return m_counter;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool ReferenceCounter::isReferencing() const
    {
        return m_counter != 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
