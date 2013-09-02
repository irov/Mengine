#	pragma once

#	include "Kernel/Node.h"

#	include "HotspotMousePickerAdapter.h"

#	include "Core/Polygon.h"

namespace Menge
{
	class Arrow;

	class HotSpot
		: public Node
	{
	public:
		HotSpot();
		~HotSpot();

    protected:
        void _setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		void setPolygon( const Polygon & _polygon );
		const Polygon & getPolygon() const;

    public:
        void setDefaultHandle( bool _handle );
        bool getDefaultHandle() const;
		    
	public:
		virtual bool testArrow( const mt::mat4f& _transform, Arrow * _arrow, const mt::mat4f& _screenTransform );

	public:
		virtual bool testRadius( const mt::mat4f& _transform, float _radius, const mt::mat4f& _screenTransform );
		virtual bool testPolygon( const mt::mat4f& _transform, const Polygon& _screenPoly, const mt::mat4f& _screenTransform );
		virtual bool testPoint( const mt::mat4f& _transform, const mt::vec2f & _p, const mt::mat4f& _screenTransform );

    public:        
        void setDebugColor( uint32 _color );
        uint32 getDebugColor() const;

	public:
		void clearPoints();

	protected:
		bool _activate() override;
		void _deactivate() override;
		void _update( float _current, float _timing ) override;

		//void _invalidateBoundingBox() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _setEventListener( PyObject * _listener ) override;

	protected:
		Polygon m_polygon;
		Polygon m_polygonWM;
		Polygon m_polygonScreen;

//#	ifndef MENGE_MASTER_RELEASE
	protected:
		void _debugRender( RenderCameraInterface * _camera, unsigned int _debugMask ) override;

    private:
		void updateVertices_();

	protected:
		uint32 m_debugColor;

        HotspotMousePickerAdapter m_mousePickerAdapter;

        TVectorRenderVertex2D m_vertexDebugPolygon;
        TVectorIndices m_indicesDebugPolygon;
                
//#	endif
	};
}
