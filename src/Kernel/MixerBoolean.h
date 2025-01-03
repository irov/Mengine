#pragma once

#include "Interface/MixerBooleanInterface.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MixerBoolean
        : public MixerBooleanInterface
    {
        DECLARE_FACTORABLE( MixerBoolean );

    public:
        MixerBoolean();
        ~MixerBoolean();

    public:
        void setValue( const ConstString & _type, bool _value ) override;
        bool getValue( const ConstString & _type ) const override;

    public:
        bool mixValue() const override;

    protected:
        struct Element
        {
            ConstString type;
            bool value;
        };

        typedef Vector<Element> VectorElement;
        VectorElement m_mixer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MixerBoolean, MixerBooleanInterface> MixerBooleanPtr;
    //////////////////////////////////////////////////////////////////////////
}