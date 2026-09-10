#pragma once

#include "Interface/AttributionProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class iOSAttributionProvider
        : public AttributionProviderInterface
        , public Factorable
    {
    public:
        iOSAttributionProvider();
        ~iOSAttributionProvider() override;

    protected:
        void onAttributionChanged( const ConstString & _name, EAttributionType _type, const ParamVariant & _value ) override;
    };
    //////////////////////////////////////////////////////////////////////////
}
