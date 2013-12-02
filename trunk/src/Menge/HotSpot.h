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
		DECLARE_VISITABLE();

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
		bool isMousePickerOver() const;

	public:
		MousePickerTrapInterface * getPickerTrap();
		    
	public:
		virtual bool testPoint( const mt::mat4f& _transform, const mt::vec2f & _point );
		virtual bool testRadius( const mt::mat4f& _transform, const mt::vec2f & _point, float _radius );
		virtual bool testPolygon( const mt::mat4f& _transform, const mt::vec2f & _point, const Polygon & _polygon );

    public:        
        void setDebugColor( uint32_t _color );
        uint32_t getDebugColor() const;

	public:
		void clearPoints();

	protected:
		bool _activate() override;
		void _afterActivate() override;
		void _deactivate() override;

		//void _invalidateBoundingBox() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _setEventListener( PyObject * _listener ) override;

	protected:
		void _localHide( bool _value ) override;
		void _freeze( bool _value ) override;

	protected:
		void activatePicker_();
		void deactivatePicker_();

	protected:
		Polygon m_polygon;

	protected:
		void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;

	protected:
        HotspotMousePickerAdapter m_mousePickerAdapter;

		uint32_t m_debugColor;
	};
}
