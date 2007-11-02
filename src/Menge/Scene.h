#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "Scriptable.h"

#	include "Layer.h"

#	include "InputHandler.h"

namespace Menge
{
	class Camera2D;

	class Scene
		: public NodeCore
		, public NodeChildren<Layer>
		, public InputHandler
	{
		OBJECT_DECLARE( Scene )
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
		void loader( TiXmlElement *_xml) override;
		void render();

	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

	protected:
		bool _activate() override;
		void _update( size_t _timing, const Viewport & _viewport ) override;

		void _addChildren( Layer * _layer ) override;

	private:
		bool m_isSubScene;
		Layer * m_mainLayer;
		
	};
}