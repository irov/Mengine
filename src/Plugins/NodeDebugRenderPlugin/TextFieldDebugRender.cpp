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
    void TextFieldDebugRender::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const TextField * _textField, bool _hide )
    {
        if( _hide == true )
        {
            return;
        }

        const ConstString & textId = _textField->getTextId();
        const String & text = _textField->getText();

        if( textId.empty() == true && text.empty() == true )
        {
            return;
        }

        const TransformationInterface * transformation = _textField->getTransformation();

        const mt::mat4f & wm = transformation->getWorldMatrix();

        {
            mt::vec2f size;
            _textField->calcTotalTextSize( &size );

            Viewport vp;
            _textField->calcTotalTextViewport( &vp );

            mt::vec2f v0( vp.begin.x, vp.begin.y );
            mt::vec2f v1( vp.end.x, vp.begin.y );
            mt::vec2f v2( vp.end.x, vp.end.y );
            mt::vec2f v3( vp.begin.x, vp.end.y );

            Polygon polygon;
            polygon.append( v0 );
            polygon.append( v1 );
            polygon.append( v2 );
            polygon.append( v3 );

            Helper::nodeDebugRenderPolygon( _renderPipeline, _context, wm, polygon, 0xFF0000FF, MENGINE_DOCUMENT_FORWARD );
        }

        {
            ColorValue_ARGB color = Helper::makeRGBAF( 0.f, 1.f, 0.f, 1.f );

            Helper::nodeDebugRenderPoint( _renderPipeline, _context, wm, mt::vec2f( 0.f, 0.f ), color, 10.f, MENGINE_DOCUMENT_FORWARD );
        }

        float maxLength = _textField->calcMaxLength();

        if( maxLength != 1.f )
        {
            ETextHorizontAlign horizontAlign = _textField->getHorizontAlign();

            float start_x = 0.f;
            float finish_x = 0.f;

            switch( horizontAlign )
            {
            case ETFHA_LEFT:
                {
                    start_x = 0.f;
                    finish_x = maxLength;
                }break;
            case ETFHA_CENTER:
                {
                    start_x = -maxLength * 0.5f;
                    finish_x = maxLength * 0.5f;
                }break;
            case ETFHA_RIGHT:
                {
                    start_x = -maxLength;
                    finish_x = 0.f;
                }break;
            case ETFHA_JUSTIFY:
                {
                    start_x = 0.f;
                    finish_x = maxLength;
                }break;
            }

            float halfLength = 20.f;

            ColorValue_ARGB color = Helper::makeRGBAF( 0.f, 1.f, 1.f, 1.f );

            Helper::nodeDebugRenderLineByToPoints( _renderPipeline, _context, wm, mt::vec2f( start_x, -halfLength ), mt::vec2f( start_x, halfLength ), color, MENGINE_DOCUMENT_FORWARD );
            Helper::nodeDebugRenderLineByToPoints( _renderPipeline, _context, wm, mt::vec2f( finish_x, -halfLength ), mt::vec2f( finish_x, halfLength ), color, MENGINE_DOCUMENT_FORWARD );
        }

        if( _textField->getAutoScale() == true )
        {
            const mt::vec2f & anchor = _textField->getAnchorPercent();

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