#	pragma once

#	include "Node.h"
#	include "ResourceWindow.h"
#	include "FixedVertices.h"

namespace Menge
{
	class Window
		: public Node
		, public FixedVertices<MAX_WINDOW_ELEMENTS * 4>
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

	protected:
		void loader( XmlElement * xml ) override;

	protected:
		bool _compile() override;
		void _release() override;
		void _render( Camera2D * _camera ) override;
		void _updateBoundingBox( mt::box2f& _boundingBox ) override;
		void _invalidateWorldMatrix() override;
		void _invalidateColor() override;

	protected:
		void _updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices ) override;

	protected:
		String m_resourceName;
		ResourceWindow* m_resource;

		mt::vec2f m_clientSize;

		mt::vec2f m_initialSizes[MAX_WINDOW_ELEMENTS];
		
		Material * m_material[MAX_WINDOW_ELEMENTS];
		Texture * m_textures[MAX_WINDOW_ELEMENTS];
	};
}	// namespace Menge
