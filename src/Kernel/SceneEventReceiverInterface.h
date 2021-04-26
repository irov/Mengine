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
    typedef IntrusivePtr<SceneEventReceiverInterface> SceneEventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_APP_MOUSE_LEAVE );
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_APP_MOUSE_ENTER );
    EVENTATION_TYPEID( SceneEventReceiverInterface, EVENT_SCENE_FOCUS );
    //////////////////////////////////////////////////////////////////////////
}
