#pragma once

#include "Interface/AttributionProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidAttributionProvider
        : public AttributionProviderInterface
        , public Factorable
    {
    public:
        AndroidAttributionProvider();
        ~AndroidAttributionProvider() override;

    protected:
        void onAttributionChanged( const ConstString & _name, EAttributionType _type, const ParamVariant & _value ) override;
    };
    //////////////////////////////////////////////////////////////////////////
}
