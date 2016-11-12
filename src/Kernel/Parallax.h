#	pragma once

#	include "Kernel/Node.h"

#	include "Kernel/RenderCameraProxy.h"

namespace Menge
{
	class Parallax
		: public Node
	{
		DECLARE_VISITABLE( Node )

	public:
		Parallax();

	public:
		void setParallaxFactor( const mt::vec3f & _parallax );
		const mt::vec3f & getParallaxFactor() const;

	protected:
		void render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, unsigned int _debugMask ) override;

	protected:
		mt::vec3f m_parallaxFactor;

		RenderCameraProxy m_parallaxCamera;
	};
}
