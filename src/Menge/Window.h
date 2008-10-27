#	pragma once

#	include "Node.h"
#	include "ResourceWindow.h"

namespace Menge
{
	class Window
		: public Node
	{
		OBJECT_DECLARE( Window );
	public:
		Window();
		virtual ~Window();

		void loader( XmlElement * xml ) override;

		void invalidateQuads();
		void setClientSize( const mt::vec2f& _clientSize );
		const mt::vec2f getClientSize() const;
		mt::vec2f getWindowSize() const;

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

		void rebuildWindow_();
	};
}	// namespace Menge