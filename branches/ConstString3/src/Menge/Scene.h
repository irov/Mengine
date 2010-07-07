#	pragma once

#	include "Node.h"
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
		: public Node
		, public Reference
		, public MousePickerAdapter
	{
	public:
		Scene();
		~Scene();

	public:
		void layerAppend( const ConstString& _layer, Node * _node );
		void layerRemove( Node* _node );
		void layerHide( const ConstString& _layer, bool _value ); // depricated
		mt::vec2f screenToLocal( const ConstString& _layerName, const mt::vec2f& _point );

		void setMainLayer( Layer * _layer );
		Layer * getMainLayer();

		Node * getNode( const ConstString& _name ); // depricated
		Camera2D* getCamera();

		ScheduleManager * getScheduleManager();
		
	public:
		void setParentScene( Scene * _scene );
		bool isSubScene() const;

	protected:
		bool onEnter() override;
		void onLeave() override;

	protected:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

	protected:
		bool pick( Arrow * _arrow ) override;
		bool _pickerActive() const override;

	public:
		const mt::vec2f & getLayerSize( const ConstString& _name ); // depricated

	public:
		void loader( BinParser * _parser ) override;
		void loaderScene_( BinParser * _parser );

	public:
		void setPhysicsWorld( const mt::vec4f & _box );
		bool createPhysicsWorld();

	public:
		void onMouseLeave();
		void onMouseEnter();
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

		void _addChildren( Node * _layer ) override;

	protected:
		Layer * getLayer_( const ConstString& _name );

	public:
		void renderSelf(); //depricated
		void render( Camera2D * _camera ) override;
		void setRenderTarget( const ConstString& _cameraName, const mt::vec2f& _size );
		const ConstString & getRenderTarget() const;
		void blockInput( bool _block );
		bool getBlockInput() const;
		void setCameraPosition( float _x, float _y );
		void enableCameraFollowing( bool _enable, float _force );
		void setCameraTarget( Node* _target );
		void setCameraBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower );
		void setPhysicsCanSleep( bool _canSleep );

	private:
		Scene * m_parentScene;
		Layer * m_mainLayer;

		mt::vec2f m_offsetPosition;

		bool m_physWorld2D;
		mt::vec2f m_gravity2D;
		mt::vec4f m_physWorldBox2D;

		ConstString m_rtName;
		mt::vec2f m_rtSize;

		bool m_eventOnUpdate;
		bool m_blockInput;

		Camera2D* m_camera2D;

		ScheduleManager* m_scheduleManager;
		bool m_physicCanSleep;
	};
}
