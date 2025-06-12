#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/UpdateContext.h"

#ifndef MENGINE_MIXER_VALUE_DEFAULT_SPEED
#define MENGINE_MIXER_VALUE_DEFAULT_SPEED 0.0005f
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MixerAveragingInterface
        : public ServantInterface
    {
    public:
        virtual void setValue( const ConstString & _type, float _value, float _weight, float _from, float _speed, bool _force ) = 0;
        virtual float getValue( const ConstString & _type ) const = 0;

    public:
        virtual float mixValue() const = 0;

    public:
        virtual bool update( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MixerAveragingInterface> MixerAveragingInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}