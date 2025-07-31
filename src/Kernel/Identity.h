#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/Mixin.h"

#include "Config/Typedef.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Identity
        : public Mixin
    {
    public:
        Identity();
        ~Identity() override;

    public:
        void setName( const ConstString & _name );
        MENGINE_INLINE const ConstString & getName() const;

    protected:
        ConstString m_name;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Identity> IdentityPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & Identity::getName()const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
}
