#pragma once

#include "HotSpot.h"

namespace Mengine
{
	class HotSpotCircle
		: public HotSpot
	{
	public:
		HotSpotCircle();
		~HotSpotCircle() override;

	public:
		void setRadius( float _radius );
		float getRadius() const;
		
		void setEllipse( float _ellipse );
		float getEllipse() const;

	protected:
		bool testPoint( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
		bool testRadius( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const override;
		bool testPolygon( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;

	protected:
		void _debugRender( RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask ) override;

	protected:
		float m_radius;
		float m_ellipse;

		uint32_t m_enumerator;
	};
}	
