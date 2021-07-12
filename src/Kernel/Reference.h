#pragma once

#include "Config/Typedef.h"

#include "Kernel/Mixin.h"
#include "Kernel/ReferenceCounter.h"

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
        void decrementReference();

        MENGINE_INLINE uint32_t countReference() const;

    protected:
        virtual bool _incrementZero();
        virtual void _decrementZero();

    private:
        ReferenceCounter m_refcounter;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Reference> ReferencePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t Reference::countReference() const
    {
        uint32_t count = m_refcounter.getReferenceCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
}
