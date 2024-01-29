#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/UpdateContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MixerValueInterface
        : public ServantInterface
    {
    public:
        virtual void setSpeed( float _speed ) = 0;
        virtual float getSpeed() const = 0;

    public:
        virtual void setValue( const ConstString & _type, float _value, float _from, bool _force ) = 0;
        virtual float getValue( const ConstString & _type ) const = 0;

    public:
        virtual float mixValue() const = 0;

    public:
        virtual bool update( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MixerValueInterface> MixerValueInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}