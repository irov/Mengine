#	pragma once

#	include "Entity.h"
#	include "Reference.h"

#	include "Scriptable.h"
#	include "InputHandler.h"

#	include "Layer.h"

#	include "math/vec3.h"
#	include "math/vec4.h"

namespace Menge
{
	class Camera2D;
	class ResourceImage;

	class ScheduleManager;

	class Scene
		: public Entity
		, public Reference
	{
	public:
		Scene();
		~Scene();

	public:
		void setMainLayer( Layer * _layer );
		Layer * getMainLayer();
				
	public:
		Scene * getParentScene() const;
		bool isSubScene() const;

	public:
		Scene * getScene() override;
				
	public:
		void setCamera2D( Camera2D * _camera2D );

	public:
		void setPhysicsWorld( const mt::vec4f & _box );
		
	protected:
		bool createPhysicsWorld_();

	public:
		void onAppMouseLeave();
		void onAppMouseEnter();
		void onFocus( bool _focus );

	protected:
		void _render( Camera2D * _camera ) override;

	protected:
		void _addChildren( Node * _layer ) override;
		void _changeParent( Node * _oldParent, Node * _newParent ) override;

	protected:
		void _setEventListener( PyObject * _embed ) override;

	public:
		void render( Camera2D * _camera ) override;
		void setRenderTarget( const ConstString& _cameraName, const mt::vec2f& _size );
		const ConstString & getRenderTarget() const;

		void setPhysicsCanSleep( bool _canSleep );

	public:
		void renderSelf( Camera2D * _camera ); //depricated

	protected:
		Scene * m_parentScene;
		Layer * m_mainLayer;

		mt::vec2f m_offsetPosition;

		//bool m_physWorld2D;
		//bool m_physicCanSleep;
		//mt::vec2f m_gravity2D;
		//mt::vec4f m_physWorldBox2D;

		ConstString m_renderTargetName;
		mt::vec2f m_renderTargetSize;

		Camera2D * m_camera2D;

		//ScheduleManager* m_scheduleManager;		
	};
}
