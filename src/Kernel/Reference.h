#pragma once

#include "Config/Typedef.h"

#include "Kernel/ThreadGuard.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Reference
        : public Mixin
    {
    public:
        Reference();
        ~Reference() override;

    public:
        bool incrementReference();
        bool decrementReference();

        MENGINE_INLINE uint32_t countReference() const;

    protected:
        virtual bool _incrementZero();
        virtual void _decrementZero();

    protected:
        uint32_t m_refcounter;

        MENGINE_THREAD_GUARD_INIT( Reference );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Reference> ReferencePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t Reference::countReference() const
    {
        return m_refcounter;
    }
    //////////////////////////////////////////////////////////////////////////
}
