#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Params.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class EAttributionType : uint8_t
    {
        EAT_BOOLEAN = 1,
        EAT_INTEGER,
        EAT_DOUBLE,
        EAT_STRING
    };
    //////////////////////////////////////////////////////////////////////////
    class AttributionProviderInterface
        : public Mixin
    {
    public:
        virtual void onAttributionChanged( const ConstString & _name, EAttributionType _type, const ParamVariant & _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AttributionProviderInterface> AttributionProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
