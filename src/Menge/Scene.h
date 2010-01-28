#	pragma once

#	include "Node.h"
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
		: public Node
		, public Reference
	{
	public:
		Scene();
		~Scene();

	public:
		void layerAppend( const String& _layer, Node * _node );
		void layerRemove( Node* _node );
		void layerHide( const String& _layer, bool _value ); // depricated
		mt::vec2f screenToLocal( const String& _layerName, const mt::vec2f& _point );

		void setMainLayer( Layer * _layer );
		Layer * getMainLayer();

		Node * getNode( const String& _name );
		Camera2D* getCamera();

		ScheduleManager * getScheduleManager();
		
	public:
		void setParentScene( Scene * _scene );
		bool isSubScene() const;

	public:
		const mt::vec2f & getLayerSize( const String& _name ); // depricated

	public:
		void loader( XmlElement *_xml) override;
		void loaderScene_( XmlElement * _xml );

	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

		void onMouseLeave();
		void onMouseEnter();
		void onFocus( bool _focus );

	public:
		void regGlobalMouseEventable( GlobalMouseHandler * _handler );
		void unregGlobalMouseEventable( GlobalMouseHandler * _handler );

		void regGlobalKeyEventable( GlobalKeyHandler * _handler );
		void unregGlobalKeyEventable( GlobalKeyHandler * _handler );

	public:
		void addHomeless( Node * _node );
		bool isHomeless( Node * _node );

	public:
		bool compile() override;

	protected:
		void _destroy() override;

		bool _activate() override;
		void _deactivate() override;

		void _update( float _timing ) override;
		bool _compile() override;
		void _release() override;
		void _render( Camera2D * _camera ) override;

		void _addChildren( Node * _layer ) override;
		void _removeChildren( Node * _node ) override;

	protected:
		Layer * getLayer_( const String& _name );

	public:
		void renderSelf();
		void render( Camera2D * _camera ) override;
		void setRenderTarget( const String& _cameraName, const mt::vec2f& _size );
		const String& getRenderTarget() const;
		void blockInput( bool _block );
		bool getBlockInput() const;
		void setCameraPosition( float _x, float _y );
		void enableCameraFollowing( bool _enable, float _force );
		void setCameraTarget( Node* _target );
		void setCameraBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower );

	private:
		Scene * m_parentScene;
		Layer * m_mainLayer;

		mt::vec2f m_offsetPosition;

		bool m_physWorld2D;
		mt::vec2f m_gravity2D;
		mt::vec4f m_physWorldBox2D;

		String m_rtName;
		mt::vec2f m_rtSize;

		bool m_onUpdateEvent;
		bool m_blockInput;

		Camera2D* m_camera2D;

		typedef std::list<Node *> TContainerHomeless;
		TContainerChildren m_homeless;

		ScheduleManager* m_scheduleManager;

		typedef std::list<GlobalMouseHandler *> TSetGlobalMouseHandler;
		TSetGlobalMouseHandler m_setGlobalMouseHandler;

		typedef std::list<GlobalKeyHandler *> TSetGlobalKeyHandler;
		TSetGlobalKeyHandler m_setGlobalKeyHandler;
	};
}
