#	pragma once

#   include "Interface/StreamInterface.h"
#   include "Interface/VideoCodecInterface.h"

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"
#	include "Kernel/FixedVertices.h"
#	include "Kernel/Materialable.h"

#   include "ResourceVideo.h"
#	include "SurfaceVideo.h"

namespace Menge
{
	class Video
		: public Node
		, public QuadVertices
	{
	public:
		Video();
	
	public:
		void setSurfaceVideo( const SurfaceVideoPtr & _surfaceVideo );
		const SurfaceVideoPtr & getSurfaceVideo() const;
		
	protected:
		void _update( float _current, float _timing ) override;
		void _render( const RenderObjectState * _state ) override;
		
    protected:
		bool _compile() override;
		void _release() override;
		
	protected:
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
		void _invalidateColor() override;
		
	protected:
		void _updateVertices( RenderVertex2D * _vertices, unsigned char _invalidateVertices ) const override;

	protected:
		SurfaceVideoPtr m_surfaceVideo;
	};
}
