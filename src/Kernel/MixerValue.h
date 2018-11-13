#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/ValueFollowerLinear.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct UpdateContext;
    //////////////////////////////////////////////////////////////////////////
    class MixerValue
    {
    public:
        MixerValue();        
        ~MixerValue();

    public:
        void setSpeed( float _speed );
        float getSpeed() const;

        void setDefault( float _default );
        float getDefault() const;

    public:
        void setValue( const ConstString & _type, float _value, float _from, bool _force );
        float getValue( const ConstString & _type ) const;

    public:
        float mixValue() const;

    public:
        bool update( const UpdateContext * _context );

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
}