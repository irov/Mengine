#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MixerBooleanInterface
        : public ServantInterface
    {
    public:
        virtual void setValue( const ConstString & _type, bool _value ) = 0;
        virtual bool getValue( const ConstString & _type ) const = 0;

    public:
        virtual bool mixValue() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MixerBooleanInterface> MixerBooleanInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}