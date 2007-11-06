#	pragma once

#	include "Layer.h"

#	include "NodeSinglethon.h"

namespace Menge
{
	class LayerScene
		: public Layer
		, public NodeSinglethon
	{
		OBJECT_DECLARE(LayerScene);

	public:
		LayerScene();

	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

	public:
		void loader( TiXmlElement * _xml ) override;

	public:
		Node * getChildren( const std::string & _name, bool _recursion ) override;

	public:
		void render() override;

	public:
		void update( size_t _timing ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

	public:
		std::string m_sceneName;
		Scene * m_subScene;
	};
}
