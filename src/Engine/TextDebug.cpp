#include "TextDebug.h"

#include "Interface/TextServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include "TextRenderChunk.h"
#include "TextLine.h"

#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/Document.h"

#include <stdarg.h>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static void drawTextDebug2( const mt::vec2f & _pos, const TextFontInterfacePtr & _font, uint32_t _argb, const String & _message )
        {
            U32String cacheText = _font->prepareText( _message );

            if( _font->prepareGlyph( cacheText ) == false )
            {
                return;
            }

            TextLine tl( 0, 0.f );
            tl.initialize( 0, _font, cacheText );

            uint32_t vertexCount = 0;

            const VectorCharData & charsData = tl.getCharsData();

            vertexCount += charsData.size() * 4;

            VectorRenderVertex2D & vertexData = RENDER_SERVICE()
                ->getDebugRenderVertex2D( vertexCount );

            typedef Vector<TextRenderChunk> VectorChunks;
            VectorChunks chunks;

            TextRenderChunk new_chunk;
            new_chunk.vertex_begin = 0;
            new_chunk.vertex_count = 0;
            new_chunk.material = nullptr;

            mt::vec2f offset( 0.f, 0.f );

            for( const CharData & cd : charsData )
            {
                if( cd.texture == nullptr )
                {
                    tl.advanceCharOffset( cd, 1.f, offset );

                    continue;
                }

                for( uint32_t i = 0; i != 4; ++i )
                {
                    RenderVertex2D v;

                    tl.calcCharPosition( cd, offset, 1.f, i, v.position );

                    v.position.x += _pos.x;
                    v.position.y += _pos.y;
                    v.color = _argb;
                    v.uv[0] = cd.uv[i];

                    vertexData.emplace_back( v );
                }

                tl.advanceCharOffset( cd, 1.f, offset );

                RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
                    ->getMaterial3( EM_DEBUG, PT_TRIANGLELIST, 1, &cd.texture, MENGINE_DOCUMENT_FUNCTION );

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

            RenderContext context;
            context.viewport = PLAYER_SERVICE()
                ->getRenderViewport();
            context.camera = PLAYER_SERVICE()
                ->getRenderCamera();
            context.transformation = nullptr;
            context.scissor = nullptr;
            context.target = nullptr;

            const VectorRenderVertex2D::value_type * vertices = vertexData.data();

            for( const TextRenderChunk & chunk : chunks )
            {
                const VectorRenderVertex2D::value_type * chunk_vertices = vertices + chunk.vertex_begin;

                if( chunk_vertices[0].color == 16777215 )
                {
                    continue;
                }

                RENDER_SERVICE()
                    ->addDebugRenderQuad( &context, chunk.material, chunk_vertices, chunk.vertex_count );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void drawTextDebug( const mt::vec2f & _pos, const ConstString & _fontName, const Color & _color, const Char * _format, ... )
        {
            ConstString fontName = _fontName;

            if( _fontName.empty() == true )
            {
                fontName = TEXT_SERVICE()
                    ->getDefaultFontName();
            }

            const TextFontInterfacePtr & font = TEXT_SERVICE()
                ->getFont( fontName );

            if( font == nullptr )
            {
                return;
            }

            uint32_t fontLayoutCount = font->getLayoutCount();

            if( fontLayoutCount != 1 )
            {
                return;
            }

            uint32_t argb = _color.getAsARGB();

            va_list args;
            va_start( args, _format );

            Char msg[2048] = { 0 };
            vsnprintf( msg, 2047, _format, args );

            drawTextDebug2( _pos, font, argb, msg );

            va_end( args );
        }
    }
}