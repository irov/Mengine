#pragma once

#include "Interface/MixerAveragingInterface.h"

#include "Kernel/ValueFollowerLinear.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MixerAveraging
        : public MixerAveragingInterface
    {
        DECLARE_FACTORABLE( MixerAveraging );

    public:
        MixerAveraging();
        ~MixerAveraging();

    public:
        void setValue( const ConstString & _type, float _value, float _weight, float _from, float _speed, bool _force ) override;
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
            float weight;
        };

        typedef Vector<Element> VectorElement;
        VectorElement m_mixer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MixerAveraging, MixerAveragingInterface> MixerAveragingPtr;
    //////////////////////////////////////////////////////////////////////////
}