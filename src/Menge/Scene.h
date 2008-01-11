#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "Scriptable.h"

#	include "Layer.h"

#	include "InputHandler.h"

#	include "Allocator2D.h"

namespace Menge
{
	class Camera2D;

	class Scene
		: public NodeCore
		, public NodeRenderable
		, public NodeChildren<Layer>
		, public InputHandler
	{
	public:
		Scene();
		~Scene();

	public:
		void layerAppend( const std::string & _layer, Node * _node );

		void setMainLayer( Layer * _layer );
		Layer * getMainLayer();

		Node * getNode(const std::string & _name );

	public:
		void setParentScene( Scene * _scene );
		bool isSubScene() const;

	public:
		const mt::vec2f & getLayerSize( const std::string & _name );

	public:
		void loader( XmlElement *_xml) override;
		void loaderScene_( XmlElement * _xml );

	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		void _update( float _timing ) override;

		void _addChildren( Layer * _layer ) override;

	private:
		bool m_isSubScene;
		Layer * m_mainLayer;

		mt::vec2f m_offsetPosition;
	};
}