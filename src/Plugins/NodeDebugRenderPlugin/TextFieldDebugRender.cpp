#include "TextFieldDebugRender.h"

#include "NodeDebugRenderHelper.h"

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
    void TextFieldDebugRender::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, TextField * _node )
    {
        Viewport viewport;
        if( _node->calcTextViewport( viewport ) == false )
        {
            return;
        }

        mt::vec2f b = viewport.begin;
        mt::vec2f e = viewport.end;

        mt::vec2f v0( b.x, b.y );
        mt::vec2f v1( e.x, b.y );
        mt::vec2f v2( e.x, e.y );
        mt::vec2f v3( b.x, e.y );

        Polygon polygon;
        polygon.append( v0 );
        polygon.append( v1 );
        polygon.append( v2 );
        polygon.append( v3 );

        const mt::mat4f & wm = _node->getWorldMatrix();

        Helper::nodeDebugRenderPolygon( _renderPipeline, _context, wm, polygon, 0xFF0000FF, MENGINE_DOCUMENT_FORWARD );
    }
}