#pragma once

#include "Interface/Interface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SceneDataProviderInterface
        : public Interface
    {
    public:
        virtual void setUpdateSceneFlag( bool _flag ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SceneDataProviderInterface> SceneDataProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}