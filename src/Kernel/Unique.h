#pragma once

#include "Kernel/Mixin.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Unique
        : public Mixin
    {
    public:
        Unique();
        ~Unique() override;

    public:
        void setUniqueIdentity( UniqueId _uniqueIdentity );
        MENGINE_INLINE UniqueId getUniqueIdentity() const;

    protected:
        UniqueId m_uniqueIdentity;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Unique> UniquePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE UniqueId Unique::getUniqueIdentity() const
    {
        return m_uniqueIdentity;
    }
    //////////////////////////////////////////////////////////////////////////
}
