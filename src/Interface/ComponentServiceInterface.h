#pragma once

#include "Interface/ServiceInterface.h"

#include <typeinfo>
#include <typeindex>

namespace Mengine
{
    class ComponentServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ComponentService" )

    public:
        template<class T>
        uint32_t getComponentId()
        {
            static uint32_t id = this->makeComponentId( std::type_index( typeid(T) ) );

            return id;
        }

    protected:
        virtual uint32_t makeComponentId( const std::type_index & _index ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define COMPONENT_SERVICE()\
    ((Mengine::ComponentServiceInterface *)SERVICE_GET(Mengine::ComponentServiceInterface))
//////////////////////////////////////////////////////////////////////////