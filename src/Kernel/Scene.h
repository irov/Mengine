#pragma once

#include "Kernel/Entity.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Layer.h"

#include "math/vec3.h"
#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESceneEventFlag
    {
        EVENT_SCENE_APP_MOUSE_LEAVE = __EVENT_ENTITY_LAST__,
        EVENT_SCENE_APP_MOUSE_ENTER,
        EVENT_SCENE_FOCUS,
        __EVENT_SCENE_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class SceneEventReceiverInterface
        : public EntityEventReceiverInterface
    {
    public:
        virtual bool onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual bool onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual bool onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SceneEventReceiverInterface> SceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_APP_MOUSE_LEAVE );
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_APP_MOUSE_ENTER );
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_FOCUS );
    //////////////////////////////////////////////////////////////////////////
    class Scene
        : public Entity
    {
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
