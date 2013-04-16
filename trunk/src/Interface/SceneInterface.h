#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    class Scene;

    class SceneServiceInterface
        : virtual public ServiceInterface
        , virtual public PrototypeManagerInterface
    {
        SERVICE_DECLARE("SceneService")

    public:
        virtual Scene * createScene( const ConstString & _name ) = 0;
    };

#   define SCENE_SERVICE( serviceProvider )\
    (Menge::getService<Menge::SceneServiceInterface>(serviceProvider))
}