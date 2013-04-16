#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/FixedVertices.h"

#	include "ResourceWindow.h"


namespace Menge
{
    class ResourceServiceInterface;
    class RenderServiceInterface;

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
		
        const mt::vec2f getClientSize() const;		
        mt::vec2f getWindowSize() const;
		const mt::vec2f& getTileSize( int _tile ) const;

		void setResourceWindow( const ConstString & _resource );

		bool hasBackground() const;

	protected:
		bool _compile() override;
		void _release() override;
		void _render( RenderCameraInterface * _camera ) override;
		void _updateBoundingBox( mt::box2f& _boundingBox ) override;
		void _invalidateWorldMatrix() override;
		void _invalidateColor() override;

	protected:
		void _updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices ) override;

	protected:
		ConstString m_resourceName;
		ResourceWindow* m_resource;

		mt::vec2f m_clientSize;

		mt::vec2f m_initialSizes[ResourceWindow_Count];
		
		bool m_hasBackGround;
		
		const RenderMaterial * m_material[ResourceWindow_Count];
		const RenderTextureInterface * m_textures[ResourceWindow_Count];
	};
}	// namespace Menge
