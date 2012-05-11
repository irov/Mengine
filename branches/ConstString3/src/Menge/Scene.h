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
		void onAppMouseLeave();
		void onAppMouseEnter();
		void onFocus( bool _focus );

	protected:
		void _render( RenderCameraInterface * _camera ) override;

	protected:
		void _addChildren( Node * _layer ) override;
		void _changeParent( Node * _oldParent, Node * _newParent ) override;

	protected:
		void _setEventListener( PyObject * _embed ) override;

	protected:
		Scene * m_parentScene;
		Layer * m_mainLayer;

		mt::vec2f m_offsetPosition;	
	};
}
