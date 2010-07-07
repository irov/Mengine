#	pragma once

#	include "Layer.h"

namespace Menge
{
	class LayerScene
		: public Layer
	{
	public:
		LayerScene();

	public:
		void loader( BinParser * _parser ) override;
		void loadScene_( const ConstString & _scene );

	public:
		Node * getChildren( const ConstString& _name, bool _recursive ) const override;
		bool isChildren( Node * _node, bool _recursive ) const override;

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
		ConstString m_sceneName;
		Scene * m_subScene;
	};
}
