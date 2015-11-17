#	pragma once

#	include "HotSpot.h"

namespace Menge
{
	class Arrow;

	class HotSpotPolygon
		: public HotSpot
	{
		DECLARE_VISITABLE();

	public:
		HotSpotPolygon();
		~HotSpotPolygon();

	public:
		void setPolygon( const Polygon & _polygon );
		const Polygon & getPolygon() const;

	public:
		bool testPoint( const mt::vec2f & _point ) const override;
		bool testRadius( const mt::vec2f & _point, float _radius ) const override;
		bool testPolygon( const mt::vec2f & _point, const Polygon & _polygon ) const override;

	public:
		void clearPoints();

	protected:
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;

	protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void invalidatePolygonWM();
		void updatePolygonWM_() const;

	public:
		inline const Polygon & getPolygonWM() const;

	protected:
		void _debugRender( const RenderObjectState * _state, unsigned int _debugMask ) override;

	protected:        
		Polygon m_polygon;
		mutable Polygon m_polygonWM;
		mutable Polygon m_polygonTemp;

		mutable bool m_invalidatePolygonWM;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const Polygon & HotSpotPolygon::getPolygonWM() const
	{
		if( m_invalidatePolygonWM == true )
		{
			this->updatePolygonWM_();
		}

		return m_polygonWM;
	}
}
