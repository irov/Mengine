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
	class Camera;

	class Layer
		: public Node
	{
	public:
		Layer();

	public:
		void setMain( bool value );
		bool isMain() const;

		void setScene( Scene * _scene );
		Scene * getScene();

		void setSize( const mt::vec2f & _size );
		const mt::vec2f & getSize() const;

		virtual void setRenderViewport( const Viewport & _viewport );
		const Viewport & getRenderViewport() const;

		virtual bool testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const;
		virtual bool testHotspot( const Viewport & _viewport, HotSpot * _layerspaceHotspot, HotSpot * _screenspaceHotspot ) const;
		virtual bool testHotspot( const Viewport& _viewport, HotSpot* _layerspaceHotspot, const mt::vec2f& _point ) const;

		virtual void _updateBoundingBox( mt::box2f& _boundingBox ) override;

	public:
		virtual void setRenderTarget( const std::string& _cameraName );
		

		virtual mt::vec2f calcScreenPosition( const Viewport& _viewport, Node* _node ) const;

		virtual Camera* getCamera();

	public:
		void loader( XmlElement * _xml ) override;

		void _render( Camera2D * _camera ) override;

	protected:
		bool m_main;
		mt::vec2f m_size;		

		Viewport m_renderViewport;
		//String m_renderTarget;

		Scene * m_scene;
	};
}
