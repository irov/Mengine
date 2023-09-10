#include "TextDebug.h"

#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"

#include "TextRenderChunk.h"
#include "TextLine.h"

#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/StringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Assertion.h"

#include "Config/StdIO.h"
#include "Config/StdArg.h"
#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static void drawTextDebug2( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::vec2f & _pos, const FontInterfacePtr & _font, uint32_t _argb, const Char * _message, size_t _size, const DocumentInterfacePtr & _doc )
        {
            U32String cacheText;
            if( _font->prepareText( _message, _size, &cacheText ) == false )
            {
                return;
            }

            if( _font->prepareGlyph( cacheText, _doc ) == false )
            {
                return;
            }

            const VectorU32String & lineDelims = TEXT_SERVICE()
                ->getLineDelims();

            VectorU32String drawTexts;
            Helper::u32split2( &drawTexts, cacheText, true, lineDelims );

            float fontLineOffset = _font->getLineOffset();
            float fontBearingYA = _font->getFontBearingYA();

            float offset_y = 0.f;

            for( const U32String & text : drawTexts )
            {
                TextLine tl( 0, 0.f );
                tl.initialize( 0, _font, text );

                uint32_t vertexCount = 0;

                const VectorTextCharData & charsData = tl.getCharsData();

                uint32_t charsDataCount = (uint32_t)charsData.size();

                vertexCount += charsDataCount * 4;

                VectorRenderVertex2D & vertexData = RENDER_SERVICE()
                    ->getDebugRenderVertex2D( vertexCount );

                vertexData.clear();

                typedef Vector<TextRenderChunk> VectorChunks;
                VectorChunks chunks;

                TextRenderChunk new_chunk;
                new_chunk.vertex_begin = 0;
                new_chunk.vertex_count = 0;
                new_chunk.material = nullptr;

                mt::vec2f offset( 0.f, 0.f );

                for( const TextCharData & cd : charsData )
                {
                    if( cd.texture == nullptr )
                    {
                        tl.advanceCharOffset( cd, 1.f, &offset );

                        continue;
                    }

                    for( uint32_t i = 0; i != 4; ++i )
                    {
                        RenderVertex2D v;

                        tl.calcCharPosition( cd, offset, 1.f, i, &v.position );

                        v.position.x += _pos.x;
                        v.position.y += _pos.y + offset_y;
                        v.color = _argb;
                        v.uv[0] = cd.uv[i];

                        vertexData.emplace_back( v );
                    }

                    tl.advanceCharOffset( cd, 1.f, &offset );

                    RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
                        ->getMaterial3( EM_TEXTURE_BLEND, PT_TRIANGLELIST, &cd.texture, 1, _doc );

                    if( new_chunk.material == material )
                    {
                        new_chunk.vertex_count += 4;
                    }
                    else if( new_chunk.material == nullptr )
                    {
                        new_chunk.vertex_begin = 0;
                        new_chunk.vertex_count = 4;
                        new_chunk.material = material;
                    }
                    else
                    {
                        chunks.emplace_back( new_chunk );

                        new_chunk.vertex_begin = new_chunk.vertex_begin + new_chunk.vertex_count;
                        new_chunk.vertex_count = 4;
                        new_chunk.material = material;
                    }
                }

                if( new_chunk.vertex_count != 0 )
                {
                    chunks.emplace_back( new_chunk );
                }

                const VectorRenderVertex2D::value_type * vertices = vertexData.data();

                for( const TextRenderChunk & chunk : chunks )
                {
                    const VectorRenderVertex2D::value_type * chunk_vertices = vertices + chunk.vertex_begin;

                    if( chunk_vertices[0].color == 16777215 )
                    {
                        continue;
                    }

                    _renderPipeline->addDebugRenderQuad( _context, chunk.material, chunk_vertices, chunk.vertex_count, _doc );
                }

                offset_y += fontBearingYA + fontLineOffset;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void drawTextDebug( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::vec2f & _pos, const FontInterfacePtr & _font, const Color & _color, const DocumentInterfacePtr & _doc, const Char * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            Char msg[2048] = {'\0'};
            int32_t size_vsnprintf = MENGINE_VSNPRINTF( msg, 2048, _format, args );

            MENGINE_VA_LIST_END( args );

            MENGINE_ASSERTION_FATAL( size_vsnprintf >= 0, "invalid string format '%s'"
                , _format
            );

            ColorValue_ARGB argb = _color.getAsARGB();

            FontInterfacePtr font = _font;

            if( font == nullptr )
            {
                font = FONT_SERVICE()
                    ->getDefaultFont();

                if( font == nullptr )
                {
                    return;
                }
            }

            if( font->compileFont() == false )
            {
                return;
            }
           
            uint32_t fontLayoutCount = font->getLayoutCount();

            if( fontLayoutCount == 1 )
            {
                Helper::drawTextDebug2( _renderPipeline, _context, _pos, font, argb, msg, size_vsnprintf, _doc );
            }

            font->releaseFont();
        }
        //////////////////////////////////////////////////////////////////////////
    }
}