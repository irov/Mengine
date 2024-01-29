#pragma once

#include "Interface/MixerValueInterface.h"

#include "Kernel/ValueFollowerLinear.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MixerValue
        : public MixerValueInterface
    {
        DECLARE_FACTORABLE( MixerValue );

    public:
        MixerValue();
        ~MixerValue();

    public:
        void setSpeed( float _speed ) override;
        float getSpeed() const override;

    public:
        void setValue( const ConstString & _type, float _value, float _from, bool _force ) override;
        float getValue( const ConstString & _type ) const override;

    public:
        float mixValue() const override;

    public:
        bool update( const UpdateContext * _context ) override;

    protected:
        float m_speed;

        struct Element
        {
            ConstString type;
            ValueFollowerLinear<float> follower;
        };

        typedef Vector<Element> VectorMixerElement;
        VectorMixerElement m_mixer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MixerValue, MixerValueInterface> MixerValuePtr;
    //////////////////////////////////////////////////////////////////////////
}