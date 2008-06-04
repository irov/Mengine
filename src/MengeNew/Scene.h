#	pragma once

#	include "Node.h"

#	include "Scriptable.h"

#	include "Layer.h"

#	include "InputHandler.h"

#	include "math\vec4.h"

namespace Menge
{
	class Camera2D;

	class Scene
		: public Node		
	{
	public:
		Scene();
		~Scene();

	public:
		void layerAppend( const std::string & _layer, Node * _node );

		void setMainLayer( Layer * _layer );
		Layer * getMainLayer();

		void setPhysicWorldBox2D( const mt::vec4f & _physWorldBox2D );
		const mt::vec4f & getPhysicWorldBox2D() const;

		Node * getNode( const std::string & _name );

	public:
		void setParentScene( Scene * _scene );
		bool isSubScene() const;

	public:
		const mt::vec2f & getLayerSize( const std::string & _name );

	public:
		void loader( XmlElement *_xml) override;
		void loaderScene_( XmlElement * _xml );

		bool compile() override;

	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		void _update( float _timing ) override;
		void _release() override;

		void _addChildren( Node * _layer ) override;

	public:
		void setOffsetPosition( const mt::vec2f& _offset );
		void setRenderTarget( const std::string& _cameraName );

	protected:
		Layer * getLayer_( const std::string & _name );

	protected:
		bool m_isSubScene;
		Layer * m_mainLayer;

		mt::vec2f m_offsetPosition;

		bool m_physWorld2D;
		mt::vec2f m_gravity2D;
		mt::vec4f m_physWorldBox2D;
	};
}