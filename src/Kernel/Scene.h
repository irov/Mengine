#pragma once

#include "Kernel/Entity.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Scene
        : public Entity
    {
        DECLARE_FACTORABLE( Scene );
        DECLARE_VISITABLE( Entity );

    public:
        Scene();
        ~Scene() override;

    public:
        void onAppMouseLeave();
        void onAppMouseEnter();

    public:
        void onFocus( bool _focus );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Scene> ScenePtr;
    //////////////////////////////////////////////////////////////////////////
}
