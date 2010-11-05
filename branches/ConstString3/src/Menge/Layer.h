#	pragma once

#	include "Config/Typedef.h"

#	include "Node.h"
#	include "NodeRenderable.h"

#	include "Math/vec2.h"
#	include "Math/vec4.h"

namespace Menge
{
	class Scene;
	class HotSpot;
	class Arrow;
	class Camera;

	class Layer
		: public Node
	{
	public:
		Layer();
		~Layer();

	public:
		void setMain( bool value );
		bool isMain() const;

	public:
		void setScene( Scene * _scene );
		Scene * getScene() override;
	
	public:
		void setSize( const mt::vec2f & _size );
		const mt::vec2f & getSize() const;

	public:
		void setLayer( Layer * _layer ) override;

	public:
		virtual bool testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const;
		virtual bool testArrow( const Viewport & _viewport, HotSpot * _layerspaceHotspot, Arrow * _arrow ) const;
		virtual bool testPoint( const Viewport& _viewport, HotSpot* _layerspaceHotspot, const mt::vec2f& _point ) const;

		virtual void _updateBoundingBox( mt::box2f& _boundingBox ) override;

	public:
		virtual mt::vec2f calcScreenPosition( const Viewport& _viewport, Node* _node ) const;

		virtual Camera* getCamera();

	protected:
		void loader( BinParser * _parser ) override;

	protected:
		bool m_main;
		mt::vec2f m_size;

		Scene * m_scene;
	};
}
