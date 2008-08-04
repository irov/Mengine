#	pragma once

#	include "Layer.h"

namespace Menge
{
	class LayerScene
		: public Layer
	{
		OBJECT_DECLARE(LayerScene);

	public:
		LayerScene();

	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) override;

	public:
		void loader( XmlElement * _xml ) override;

	public:
		Node * getChildren( const std::string & _name, bool _recursion ) const override;

	public:
		void render( bool _enableDebug ) override;

	public:
		void update( float _timing ) override;

	protected:
		void _destroy() override;
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

	public:
		std::string m_sceneName;
		Scene * m_subScene;
	};
}
