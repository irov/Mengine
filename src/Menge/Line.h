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

	protected:
		void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;

	protected:
		mt::vec3f m_from;
		mt::vec3f m_to;
	};
}
