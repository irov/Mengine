#	pragma once

#	include "Entity.h"
#	include "Reference.h"

#	include "Scriptable.h"
#	include "InputHandler.h"

#	include "Layer.h"

#	include "math/vec3.h"
#	include "math/vec4.h"

#	include "MousePickerAdapter.h"

namespace Menge
{
	class Camera2D;
	class ResourceImage;

	class ScheduleManager;

	class Scene
		: public Entity
		, public Reference
		, public MousePickerAdapter
	{
	public:
		Scene();
		~Scene();

	public:
		void initialize() override;

	public:
		void setMainLayer( Layer * _layer );
		Layer * getMainLayer();

		ScheduleManager * getScheduleManager();
				
	public:
		Scene * getParentScene() const;
		bool isSubScene() const;

	public:
		Scene * getScene() override;

	protected:
		bool onMouseEnter() override;
		void onMouseLeave() override;

	protected:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell ) override;

	protected:
		bool pick( const mt::vec2f& _point, Arrow * _arrow ) override;
		bool _pickerActive() const override;

	public:
		void loader( BinParser * _parser ) override;

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

	public:
		bool compile() override;

	protected:
		void _destroy() override;

		bool _activate() override;
		void _deactivate() override;

		void _update( float _timing ) override;
		void _postUpdate( float _timing ) override;
		bool _compile() override;
		void _release() override;
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
		void blockInput( bool _block );
		bool getBlockInput() const;

		void setPhysicsCanSleep( bool _canSleep );

	public:
		void renderSelf( Camera2D * _camera ); //depricated

	protected:
		Scene * m_parentScene;
		Layer * m_mainLayer;

		mt::vec2f m_offsetPosition;

		bool m_physWorld2D;
		mt::vec2f m_gravity2D;
		mt::vec4f m_physWorldBox2D;

		ConstString m_renderTargetName;
		mt::vec2f m_renderTargetSize;

		bool m_eventOnUpdate;
		bool m_blockInput;

		Camera2D * m_camera2D;

		ScheduleManager* m_scheduleManager;
		bool m_physicCanSleep;
	};
}
