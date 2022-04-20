#include "TextFieldDebugRender.h"

#include "NodeDebugRenderHelper.h"

#include "Kernel/ColorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextFieldDebugRender::TextFieldDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextFieldDebugRender::~TextFieldDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TextFieldDebugRender::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const TextField * _node, bool _hide )
    {
        if( _hide == true )
        {
            return;
        }

        {
            const mt::vec2f & offset = _node->getTextOffset();
            const mt::vec2f & size = _node->getTextSize();

            mt::vec2f b = offset;
            mt::vec2f e = offset + size;

            mt::vec2f v0( b.x, b.y );
            mt::vec2f v1( e.x, b.y );
            mt::vec2f v2( e.x, e.y );
            mt::vec2f v3( b.x, e.y );

            Polygon polygon;
            polygon.append( v0 );
            polygon.append( v1 );
            polygon.append( v2 );
            polygon.append( v3 );

            const TransformationInterface * transformation = _node->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            Helper::nodeDebugRenderPolygon( _renderPipeline, _context, wm, polygon, 0xFF0000FF, MENGINE_DOCUMENT_FORWARD );
        }

        {
            const TransformationInterface * transformation = _node->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            ColorValue_ARGB color = Helper::makeRGBAF( 0.f, 1.f, 0.f, 1.f );

            Helper::nodeDebugRenderPoint( _renderPipeline, _context, wm, mt::vec2f( 0.f, 0.f ), color, 10.f, MENGINE_DOCUMENT_FORWARD );
        }

        if( _node->getAutoScale() == true )
        {
            const TransformationInterface * transformation = _node->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            const mt::vec2f & anchor = _node->getAnchorPercent();
            float maxLength = _node->getMaxLength();

            float start_x = -maxLength * anchor.x;
            float finish_x = start_x + maxLength;

            float halfLength = 10.f;

            ColorValue_ARGB color = Helper::makeRGBAF( 1.f, 0.f, 0.f, 1.f );

            Helper::nodeDebugRenderLineByToPoints( _renderPipeline, _context, wm, mt::vec2f( start_x, -halfLength ), mt::vec2f( start_x, halfLength ), color, MENGINE_DOCUMENT_FORWARD );
            Helper::nodeDebugRenderLineByToPoints( _renderPipeline, _context, wm, mt::vec2f( finish_x, -halfLength ), mt::vec2f( finish_x, halfLength ), color, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}