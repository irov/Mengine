#	pragma once

#	include "HotSpot.h"

namespace Menge
{
	class Arrow;

	class HotSpotPolygon
		: public HotSpot
	{
		DECLARE_VISITABLE( HotSpot );

	public:
		HotSpotPolygon();
		~HotSpotPolygon();

	public:
		void setPolygon( const Polygon & _polygon );
		const Polygon & getPolygon() const;

	public:
		bool testPoint( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
		bool testRadius( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const override;
		bool testPolygon( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;

	public:
		void clearPoints();

	public:
		void getPolygonScreen( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, mt::box2f * _bb, Polygon * _screen ) const;

	protected:
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;

	protected:
		void _debugRender( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask ) override;

	protected:        
		Polygon m_polygon;
		mutable Polygon m_polygonScreen;
		mutable Polygon m_polygonTemp;

		mutable bool m_invalidatePolygonWM;
	};
}
