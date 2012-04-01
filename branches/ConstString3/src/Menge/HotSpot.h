#	pragma once

#	include "Node.h"

#	include "MousePickerAdapter.h"
#	include "VectorVertices.h"

#	include "Node.h"
#	include "Vertex.h"

#	include "Core/Polygon.h"

namespace Menge
{
	class Arrow;

	class HotSpot
		: public Node
		, public MousePickerAdapter
#	ifndef MENGE_MASTER_RELEASE
		, public VectorVertices
#	endif
	{
	public:
		HotSpot();
		~HotSpot();

	public:
		void setPolygon( const Polygon & _polygon );
		const Polygon & getPolygon() const;
		
	protected:
		bool pick( const mt::vec2f& _point, Arrow * _arrow ) override;
		bool _pickerActive() const override;

	public:
		virtual bool testPolygon( const mt::mat3f& _transform, const Polygon& _screenPoly, const mt::mat3f& _screenTransform );
		virtual bool testPoint( const mt::vec2f & _p );

	protected:
		bool onMouseEnter() override;
		void onMouseLeave() override;

	public:
		void clearPoints();

	protected:
		void loader( BinParser *_parser ) override;

	private:
		void loaderPolygon_( BinParser *_parser );
		void endPolygon_();

	protected:
		bool _activate() override;
		void _deactivate() override;
		void _update( float _timing ) override;

		void _invalidateBoundingBox() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _setEventListener( PyObject * _listener ) override;

	protected:
		void addPoint_( const mt::vec2f & _p );

	protected:
		Polygon m_polygon;
		Polygon m_polygonWM;
		Polygon m_polygonScreen;

#	ifndef MENGE_MASTER_RELEASE
	protected:
		void _debugRender( Camera2D * _camera, unsigned int _debugMask ) override;
		void _invalidateWorldMatrix() override;

	private:
		void _updateVertices( VectorVertices::TVectorVertex2D & _vertices, unsigned char _invalidate ) override;

	protected:
		uint32 m_debugColor;
#	endif
	};
}
