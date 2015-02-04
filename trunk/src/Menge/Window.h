#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/FixedVertices.h"

#	include "ResourceWindow.h"

namespace Menge
{
	class Window
		: public Node
		, public FixedVertices<ResourceWindow_Count * 4>
	{
	public:
		Window();
		~Window();
		
	public:
		void setClientSize( const mt::vec2f& _clientSize );
		void setClientSizeClip( const mt::vec2f& _clientSize );
		void setClientSizeInTiles( const mt::vec2f& _tiles );
		
        const mt::vec2f & getClientSize() const;	

	public:
        mt::vec2f getWindowSize() const;
		const mt::vec2f getTileSize( int _tile ) const;

		void setResourceWindow( ResourceWindow * _resourceWindow );
        ResourceWindow * getResourceWindow() const;

		bool hasBackground() const;

	protected:
		bool _compile() override;
		void _release() override;
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;
		void _updateBoundingBox( mt::box2f& _boundingBox ) const override;
		void _invalidateWorldMatrix() override;
		void _invalidateColor() override;

	protected:
		void _updateVertices( RenderVertex2D * _vertices, unsigned char _invalidateVertices ) const override;

	protected:
		ResourceHolder<ResourceWindow> m_resourceWindow;

		mt::vec2f m_clientSize;
			
        struct WindowEdge
        {
            mt::vec2f initialSize;
            RenderMaterialInterfacePtr material;
        };

		WindowEdge m_edge[ResourceWindow_Count];
	};
}	// namespace Menge
