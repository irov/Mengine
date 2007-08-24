#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "Layer.h"

#	include "InputHandler.h"

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

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

	public:
		void loader( TiXmlElement *_xml) override;
		void render();

	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, float _whell ) override;

	protected:
		bool _activate() override;

	private:
		std::string m_scriptFile;
		PyObject * m_scriptScene;
	};
}