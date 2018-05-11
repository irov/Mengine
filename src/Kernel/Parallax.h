#pragma once

#include "Kernel/Node.h"

#include "Kernel/RenderCameraProxy.h"

namespace Mengine
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
		void render( RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask ) override;

	protected:
		mt::vec3f m_parallaxFactor;

		RenderCameraProxy m_parallaxCamera;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Parallax> ParallaxPtr;
    //////////////////////////////////////////////////////////////////////////
}
