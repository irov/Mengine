//#include "HotspotMousePickerVisitor.h"
//
//#include "Layer2D.h"
//#include "Layer2DIsometric.h"
//
//#include "Arrow.h"
//#include "HotSpot.h"
//
//#include "Interface/RenderSystemInterface.h"
//
//namespace Mengine
//{
//	//////////////////////////////////////////////////////////////////////////
//	HotspotMousePickerVisitor::HotspotMousePickerVisitor( HotSpot * _layerspaceHotspot, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const Resolution & _contentResolution, const mt::vec2f & _point, Arrow * _arrow )
//		: m_hotspot( _layerspaceHotspot )
//		, m_viewport( _viewport )
//		, m_camera( _camera )
//		, m_contentResolution( _contentResolution )
//		, m_point( _point )
//		, m_arrow( _arrow )
//		, m_result( false )
//    {
//    }
//	//////////////////////////////////////////////////////////////////////////
//    bool HotspotMousePickerVisitor::test( Layer * _layer )
//    {
//        _layer->visit( this );
//
//        return m_result;
//    }
//    //////////////////////////////////////////////////////////////////////////
//    void HotspotMousePickerVisitor::accept( Layer2D * _visited )
//    {
//        (void)_visited;
//
//		this->test_();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void HotspotMousePickerVisitor::accept( Layer2DIsometric * _visited )
//	{
//		(void)_visited;
//
//		this->test_();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void HotspotMousePickerVisitor::test_()
//	{		
//		//mt::vec2f wp;
//		//m_arrow->calcPointClick( m_camera, m_viewport, m_point, wp );
//
//
//    }
//}