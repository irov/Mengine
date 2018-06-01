#pragma once

#include "HotSpot.h"

namespace Mengine
{
	class Arrow;

	class HotSpotPolygon
		: public HotSpot
	{
		DECLARE_VISITABLE( HotSpot );

	public:
		HotSpotPolygon();
		~HotSpotPolygon() override;

	public:
		void setPolygon( const Polygon & _polygon );
		const Polygon & getPolygon() const;

	public:
		bool testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
		bool testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const override;
		bool testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;

	public:
		void clearPoints();

	public:
		void getPolygonScreen( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, mt::box2f * _bb, Polygon * _screen ) const;

	protected:
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;

	protected:
		void _debugRender( RenderServiceInterface * _renderService, const RenderContext * _state ) override;

	protected:        
		Polygon m_polygon;
		mutable Polygon m_polygonScreen;
		mutable Polygon m_polygonTemp;

		mutable bool m_invalidatePolygonWM;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HotSpotPolygon> HotSpotPolygonPtr;
    //////////////////////////////////////////////////////////////////////////
}
