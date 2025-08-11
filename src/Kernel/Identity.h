#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"

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
