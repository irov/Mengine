#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ComponentHubInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{    
    class ComponentServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ComponentService" )

    public:
        virtual void addComponentHub( const ConstString & _type, const ComponentHubInterfacePtr & _componentHub ) = 0;
        virtual ComponentHubInterfacePtr removeComponentHub( const ConstString & _type ) = 0;

    public:
        virtual const ComponentHubInterfacePtr & getComponentHub( const ConstString & _type ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define COMPONENT_SERVICE()\
    ((Mengine::ComponentServiceInterface *)SERVICE_GET(Mengine::ComponentServiceInterface))
//////////////////////////////////////////////////////////////////////////
