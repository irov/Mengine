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
        inline const ConstString & getName() const;

        void setType( const ConstString & _type );
        inline const ConstString & getType() const;

    protected:
        ConstString m_name;
        ConstString m_type;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Identity> IdentityPtr;
    //////////////////////////////////////////////////////////////////////////
    inline const ConstString & Identity::getName()const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const ConstString & Identity::getType() const
    {
        return m_type;
    }
}
