#pragma once

#include "Kernel/Entity.h"
#include "Kernel/Scriptable.h"
#include "Kernel/BaseEvent.h"
#include "Kernel/Layer.h"

#include "math/vec3.h"
#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum SceneEventFlag
    {
        EVENT_APP_MOUSE_LEAVE = __EVENT_ENTITY_LAST__,
        EVENT_APP_MOUSE_ENTER,
        EVENT_FOCUS,
        EVENT_ON_SUB_SCENE,
    };
    //////////////////////////////////////////////////////////////////////////
    class SceneEventReceiver
        : public EventReceiver
    {
    public:
        virtual bool onSceneAppMouseLeave( const pybind::object & _object ) = 0;
        virtual bool onSceneAppMouseEnter( const pybind::object & _object ) = 0;
        virtual bool onSceneAppFocus( const pybind::object & _object, bool _focus ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SceneEventReceiver> SceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    class Scene
		: public Entity
        , public BaseEvent
	{
        DECLARE_EVENTABLE( SceneEventReceiver );

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
    typedef IntrusivePtr<Scene> ScenePtr;
    //////////////////////////////////////////////////////////////////////////
}
