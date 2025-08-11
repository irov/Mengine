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
        MENGINE_INLINE uint32_t getReferenceCount() const;
        MENGINE_INLINE bool isReferencing() const;

    public:
        uint32_t increfReferenceCount();
        uint32_t decrefReferenceCount();

    public:
        void resetReferenceCount();

    private:
        AtomicUInt32 m_referenceCounter;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t ReferenceCounter::getReferenceCount() const
    {
        uint32_t counter = Helper::atomicLoad( m_referenceCounter );

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
