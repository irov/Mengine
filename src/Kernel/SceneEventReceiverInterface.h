#pragma once

#include "Kernel/EntityEventReceiverInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESceneEventFlag
    {
        EVENT_SCENE_APP_MOUSE_LEAVE = __EVENT_ENTITY_LAST__,
        EVENT_SCENE_APP_MOUSE_ENTER,
        EVENT_SCENE_FOCUS,
        EVENT_SCENE_RESTART_BEGIN,
        EVENT_SCENE_RESTART_END,

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

    public:
        virtual void onSceneRestartBegin( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onSceneRestartEnd( const EntityBehaviorInterfacePtr & _behavior ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SceneEventReceiverInterface> SceneEventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_APP_MOUSE_LEAVE );
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_APP_MOUSE_ENTER );
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_FOCUS );
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_RESTART_BEGIN );
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_RESTART_END );
    //////////////////////////////////////////////////////////////////////////
}
