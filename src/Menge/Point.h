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
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane ) override;

	protected:
		void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane, unsigned int _debugMask ) override;

	protected:
		Point * m_linked;
		bool m_owner;

		float m_width;

		RenderVertex2D m_vertices[4];
	};
}
