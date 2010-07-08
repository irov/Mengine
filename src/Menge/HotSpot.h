#	pragma once

#	include "Node.h"

#	include "MousePickerAdapter.h"
#	include "GlobalHandleAdapter.h"
#	include "VectorVertices.h"

#	include "VectorVertices.h"

#	include "Math/polygon.h"
#	include "Node.h"
#	include "Vertex.h"

namespace Menge
{
	class Arrow;

	class HotSpot
		: public Node
		, public MousePickerAdapter
		, public GlobalHandleAdapter
#	ifndef MENGE_MASTER_RELEASE
		, public VectorVertices
#	endif
	{
	public:
		HotSpot();
		~HotSpot();

	public:
		const mt::polygon & getPolygon() const;

	public:
		void addPoint( const mt::vec2f & _p );
		bool testPoint( const mt::vec2f & _p );
		void clearPoints();

	public:
		bool pickHotspot( HotSpot * _hotspot );

	public:
		virtual bool testPolygon( const mt::mat3f& _transform, const mt::polygon& _screenPolygon, const mt::mat3f& _screenTransform );

	protected:
		bool pick( Arrow * _arrow ) override;
		bool _pickerActive() const override;

	protected:
		bool onEnter() override;
		void onLeave() override;

	protected:
		bool isEnableGlobalHandle() const override;

	protected:
		void loader( XmlElement *_xml ) override;
		void parser( BinParser *_parser ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;
		void _update( float _timing ) override;

		void _enable() override;
		void _disable() override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _setListener( PyObject * _listener ) override;

	protected:
		mt::polygon m_polygon;

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
