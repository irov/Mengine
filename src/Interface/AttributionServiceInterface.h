#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/AttributionProviderInterface.h"

#include "Kernel/Params.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AttributionServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AttributionService" )

    public:
        virtual bool registerAttribution( const ConstString & _name, EAttributionType _type ) = 0;

    public:
        virtual bool setAttribution( const ConstString & _name, const ParamVariant & _value ) = 0;
        virtual ParamVariant getAttribution( const ConstString & _name ) const = 0;

    public:
        virtual void clearAttributions() = 0;

    public:
        virtual void addProvider( const AttributionProviderInterfacePtr & _provider ) = 0;
        virtual void removeProvider( const AttributionProviderInterfacePtr & _provider ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define ATTRIBUTION_SERVICE()\
    ((Mengine::AttributionServiceInterface *)SERVICE_GET(Mengine::AttributionServiceInterface))
//////////////////////////////////////////////////////////////////////////
