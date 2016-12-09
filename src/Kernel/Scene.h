#	pragma once

#	include "Kernel/Entity.h"
#	include "Kernel/Scriptable.h"
#	include "Kernel/Layer.h"

#	include "math/vec3.h"
#	include "math/vec4.h"

namespace Menge
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
        virtual bool onSceneAppMouseLeave() = 0;
        virtual bool onSceneAppMouseEnter() = 0;
        virtual bool onSceneAppFocus( bool _focus ) = 0;
        virtual void onSceneSubScene( class Scene * _parent ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<SceneEventReceiver> SceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    class Scene
		: public Entity
	{
        EVENT_RECEIVER( SceneEventReceiver );

	public:
		Scene();
		~Scene();

	public:
		void setMainLayer( Layer * _layer );
		Layer * getMainLayer() const;
				
	public:
		Scene * getParentScene() const;
		bool isSubScene() const;

	public:
		Scene * getScene() override;

	public:
		void onAppMouseLeave();
		void onAppMouseEnter();

    public:
		void onFocus( bool _focus );

	protected:
		void _addChild( Node * _layer ) override;
        void _removeChild( Node * _node ) override;

		void _changeParent( Node * _oldParent, Node * _newParent ) override;        

	protected:
		void _setEventListener( const pybind::dict & _embed ) override;

	protected:
		Scene * m_parentScene;
		Layer * m_mainLayer;
	};
}
