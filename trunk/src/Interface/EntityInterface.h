#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    class Entity;

    class EntityServiceInterface
        : virtual public ServiceInterface
        , virtual public PrototypeManagerInterface
    {
        SERVICE_DECLARE("EntityService")

    public:
        virtual Entity * createEntity( const ConstString & _prototype ) = 0;
    };

#   define ENTITY_SERVICE( serviceProvider )\
    (Menge::getService<Menge::EntityServiceInterface>(serviceProvider))
}