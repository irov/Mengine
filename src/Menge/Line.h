#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class Line
		: public Node
	{
	public:
		Line();

	public:
		void setFrom( const mt::vec3f & _value );
		const mt::vec3f & getFrom() const;

		void setTo( const mt::vec3f & _value );
		const mt::vec3f & getTo() const;

		void setWidth( float _width );
		float getWidth() const;

	protected:
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

	protected:
		mt::vec3f m_from;
		mt::vec3f m_to;

		float m_width;

		RenderVertex2D m_vertices[4];
	};
}
