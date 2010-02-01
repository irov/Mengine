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
		void loader( XmlElement * _xml ) override;
		void loadScene_( const std::string & _scene );

	public:
		Node * getChildren( const String& _name, bool _recursion ) const override;

	public:
		void render( Camera2D * _camera ) override;

	public:
		void update( float _timing ) override;

	public:
		void onMouseLeave();
		void onMouseEnter();
		void onFocus( bool _focus );

	public:
		Scene* getSubScene() { return m_subScene; }
		void visitChildren( Visitor * _visitor ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _destroy() override;

	public:
		String m_sceneName;
		Scene * m_subScene;
	};
}
