#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class Point
		: public Node
	{
	public:
		Point();

	public:
		void setLinkedPoint( Point * _linked );
		void removeLinkedPoint();
		Point * getLinkedPoint() const;

	public:
		void setWidth( float _width );
		float getWidth() const;

	protected:
		void _destroy() override;

	protected:
		void _render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state ) override;

	protected:
		void _debugRender( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask ) override;

	protected:
		Point * m_linked;
		bool m_owner;

		float m_width;

		RenderVertex2D m_vertices[4];
	};
}
