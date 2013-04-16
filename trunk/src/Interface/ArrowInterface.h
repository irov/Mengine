#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    class Arrow;

    class ArrowServiceInterface
        : virtual public ServiceInterface
        , virtual public PrototypeManagerInterface
    {
        SERVICE_DECLARE("ArrowService")

    public:
        virtual Arrow * createArrow( const ConstString & _prototype ) = 0;
    };

#   define ARROW_SERVICE( serviceProvider )\
    (Menge::getService<Menge::ArrowServiceInterface>(serviceProvider))
}