#	pragma once

#	include "Node.h"
#	include "Reference.h"
#	include "NodeChildren.h"

#	include "Scriptable.h"

#	include "Layer.h"

#	include "math/vec3.h"
#	include "math/vec4.h"

namespace Menge
{
	class Camera2D;
	class ResourceImage;

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
		void layerHide( const String& _layer, bool _value );
		mt::vec2f screenToLocal( const String& _layerName, const mt::vec2f& _point );

		void setMainLayer( Layer * _layer );
		Layer * getMainLayer();

		Node * getNode( const String& _name );
		Camera2D* getCamera();

	public:
		void setParentScene( Scene * _scene );
		bool isSubScene() const;

	public:
		const mt::vec2f & getLayerSize( const String& _name );

	public:
		void loader( XmlElement *_xml) override;
		void loaderScene_( XmlElement * _xml );

	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown );
		bool handleMouseMove( float _x, float _y, int _whell ) override;

		void onMouseLeave();
		void onMouseEnter();
		void onFocus( bool _focus );

	public:
		void addHomeless( Node * _node );
		bool isHomeless( Node * _node );

	protected:
		void _destroy() override;

		bool _activate() override;
		void _deactivate() override;

		void _update( float _timing ) override;
		bool _compile() override;
		void _release() override;
		void _render( unsigned int _debugMask ) override;

		void _addChildren( Node * _layer ) override;
		void _removeChildren( Node * _node ) override;

	protected:
		Layer * getLayer_( const String& _name );

	public:
		void renderSelf();
		virtual void render( unsigned int _debugMask, Camera2D* _camera ) override;
		void setRenderTarget( const String& _cameraName, const mt::vec2f& _size );
		const String& getRenderTarget() const;
		void blockInput( bool _block );
		bool getBlockInput() const;
		void setCameraPosition( float _x, float _y );

	private:
		typedef std::list<Node *> TContainerHomeless;
		TContainerChildren m_homeless;

		bool m_isSubScene;
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
	};
}
