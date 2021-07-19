//#include "ArrowDebugRender.h"
//
//#include "Interface/TransformationInterface.h"
//
//#include "NodeDebugRenderHelper.h"
//
//namespace Mengine
//{
//    //////////////////////////////////////////////////////////////////////////
//    ArrowDebugRender::ArrowDebugRender()
//    {
//    }
//    //////////////////////////////////////////////////////////////////////////
//    ArrowDebugRender::~ArrowDebugRender()
//    {
//    }
//    //////////////////////////////////////////////////////////////////////////
//    void ArrowDebugRender::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const ArrowInterfacePtr & _arrow, bool _hide )
//    {
//        MENGINE_UNUSED( _hide );
//
//        const NodePtr & node =  _arrow->getEntity();
//
//        const TransformationInterface * transformation = node->getTransformation();
//
//        const mt::mat4f & wm = transformation->getWorldMatrix();
//
//        EArrowType arrowType = _arrow->getArrowType();
//
//        switch( arrowType )
//        {
//        case EAT_POINT:
//            {
//                float radius = 2.5f;
//
//                Helper::nodeDebugRenderCircle( _renderPipeline, _context, wm, radius, 4, 0x8080FFFF, MENGINE_DOCUMENT_FORWARD );
//            }break;
//        case EAT_RADIUS:
//            {
//                float radius = _arrow->getClickRadius();
//
//                Helper::nodeDebugRenderCircle( _renderPipeline, _context, wm, radius, 32, 0x8080FFFF, MENGINE_DOCUMENT_FORWARD );
//            }break;
//        case EAT_POLYGON:
//            {
//                const Polygon & polygon = _arrow->getClickPolygon();
//
//                Helper::nodeDebugRenderPolygon( _renderPipeline, _context, wm, polygon, 0x8080FFFF, MENGINE_DOCUMENT_FORWARD );
//            }break;
//        }
//    }
//    //////////////////////////////////////////////////////////////////////////
//}