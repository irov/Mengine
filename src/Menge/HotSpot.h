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
		void setOutward( bool _value );
		bool getOutward() const;

    public:
        void setDefaultHandle( bool _handle );
        bool getDefaultHandle() const;

	public:
		bool isMousePickerOver() const;

	public:
		MousePickerTrapInterface * getPickerTrap() override;
		    
	public:
		virtual bool testPoint( const mt::vec2f & _point ) const;
		virtual bool testRadius( const mt::vec2f & _point, float _radius ) const;
		virtual bool testPolygon( const mt::vec2f & _point, const Polygon & _polygon ) const;

    public:        
        void onPickerEnter();
        void onPickerLeave();

	public:
		void clearPoints();

	protected:
		bool _activate() override;
		void _afterActivate() override;
		void _deactivate() override;

		//void _invalidateBoundingBox() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
		void _setEventListener( PyObject * _listener ) override;

	protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void _invalidHierarchy() override;

	protected:
		void invalidatePolygonWM();
		void updatePolygonWM_() const;

	public:
		inline const Polygon & getPolygonWM() const;

	protected:
		void _localHide( bool _value ) override;
		void _freeze( bool _value ) override;

	protected:
		void activatePicker_();
		void deactivatePicker_();

	protected:
		void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;

	protected:
        HotspotMousePickerAdapter m_mousePickerAdapter;

		Polygon m_polygon;
		mutable Polygon m_polygonWM;
		mutable Polygon m_polygonWMVM;

		uint32_t m_debugColor;

		bool m_outward;
		
		mutable bool m_invalidatePolygonWM;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const Polygon & HotSpot::getPolygonWM() const
	{
		if( m_invalidatePolygonWM == true )
		{
			this->updatePolygonWM_();
		}

		return m_polygonWM;
	}
}
