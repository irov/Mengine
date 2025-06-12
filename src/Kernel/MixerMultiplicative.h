#pragma once

#include "Interface/MixerMultiplicativeInterface.h"

#include "Kernel/ValueFollowerLinear.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MixerMultiplicative
        : public MixerMultiplicativeInterface
    {
        DECLARE_FACTORABLE( MixerMultiplicative );

    public:
        MixerMultiplicative();
        ~MixerMultiplicative();

    public:
        void setValue( const ConstString & _type, float _value, float _from, float _speed, bool _force ) override;
        float getValue( const ConstString & _type ) const override;

    public:
        float mixValue() const override;

    public:
        bool update( const UpdateContext * _context ) override;

    protected:
        struct Element
        {
            ConstString type;
            ValueFollowerLinear<float> follower;
        };

        typedef Vector<Element> VectorElement;
        VectorElement m_mixer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MixerMultiplicative, MixerMultiplicativeInterface> MixerMultiplicativePtr;
    //////////////////////////////////////////////////////////////////////////
}