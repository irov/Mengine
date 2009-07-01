#	pragma once

#	include "Layer.h"

namespace Menge
{
	class LayerScene
		: public Layer
	{
		FACTORABLE_DECLARE(LayerScene);

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
		Node * getChildren( const String& _name, bool _recursion ) const override;

	public:
		void render( unsigned int _debugMask, Camera2D* _camera ) override;

	public:
		void update( float _timing ) override;

		void onMouseLeave();
		void onMouseEnter();
		void onFocus( bool _focus );

		Scene* getSubScene() { return m_subScene; }
		virtual void visitChildren( Visitor * _visitor );

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

	public:
		String m_sceneName;
		Scene * m_subScene;
	};
}
