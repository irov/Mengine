#pragma once

#include "Config/Atomic.h"

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
        void setReferenceCount( uint32_t _counter );

    public:
        MENGINE_INLINE uint32_t getReferenceCount() const;
        MENGINE_INLINE bool isReferencing() const;

    public:
        uint32_t incref();
        uint32_t decref();

    public:
        void reset();

    private:
        AtomicUInt32 m_counter;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t ReferenceCounter::getReferenceCount() const
    {
        uint32_t counter = Helper::atomicLoad( m_counter );

        return counter;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool ReferenceCounter::isReferencing() const
    {
        uint32_t counter = this->getReferenceCount();

        return counter != 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
