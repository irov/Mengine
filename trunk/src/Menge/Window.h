#	pragma once

#	include "Node.h"
#	include "ResourceWindow.h"

namespace Menge
{
	class Window
		: public Node
	{
		FACTORABLE_DECLARE( Window );
	public:
		Window();
		virtual ~Window();

		void loader( XmlElement * xml ) override;

		void invalidateQuads();
		void setClientSize( const mt::vec2f& _clientSize );
		void setClientSizeClip( const mt::vec2f& _clientSize );
		void setClientSizeInTiles( const mt::vec2f& _tiles );
		const mt::vec2f getClientSize() const;
		mt::vec2f getWindowSize() const;
		const mt::vec2f& getTileSize( int _tile ) const;

	protected:
		bool _compile() override;
		void _release() override;
		void _render( unsigned int _debugMask ) override;
		void _updateBoundingBox( mt::box2f& _boundingBox ) override;
		void _invalidateWorldMatrix() override;

	protected:
		String m_resourceName;
		ResourceWindow* m_resource;
		bool m_invalidateQuads;

		mt::vec2f m_clientSize;

		struct TQuad
		{
			mt::vec2f a,b,c,d;
		};

		mt::vec2f m_initialSizes[MAX_WINDOW_ELEMENTS];
		TQuad m_quads[MAX_WINDOW_ELEMENTS];
		mt::vec2f m_UVs[MAX_WINDOW_ELEMENTS];

		Material* m_material[MAX_WINDOW_ELEMENTS];
		Vertex2D  m_vertices[MAX_WINDOW_ELEMENTS][4];

		void rebuildWindow_();
		Texture* m_textures[MAX_WINDOW_ELEMENTS];
	};
}	// namespace Menge
