#pragma once

#include "Config/Typedef.h"

#include "Kernel/ConstString.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    class Identity
        : public Mixin
    {
    public:
        Identity();
        ~Identity();

    public:
        void setName( const ConstString & _name );
        MENGINE_INLINE const ConstString & getName() const;

        void setType( const ConstString & _type );
        MENGINE_INLINE const ConstString & getType() const;

    protected:
        ConstString m_name;
        ConstString m_type;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Identity> IdentityPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & Identity::getName()const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & Identity::getType() const
    {
        return m_type;
    }
}
