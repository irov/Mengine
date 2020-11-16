#include "TTFFont.h"

#include "TTFInterface.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "TTFDataInterface.h"
#include "TTFFontTextureGlyphProvider.h"

#include "Kernel/Dataflow.h"
#include "Kernel/Stream.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "utf8.h"

#include "stdex/memorycopy.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFFont::TTFFont()
        : m_ftlibrary( nullptr )
        , m_ttfAscender( 0.f )
        , m_ttfDescender( 0.f )
        , m_ttfHeight( 0.f )
        , m_ttfBearingYA( 0.f )
        , m_ttfSpacing( 0.f )
        , m_ttfLayoutCount( 1 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFFont::~TTFFont()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setFTLibrary( FT_Library _ftlibrary )
    {
        m_ftlibrary = _ftlibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setEffect( const TextFontEffectInterfacePtr & _effect )
    {
        m_effect = _effect;
    }
    //////////////////////////////////////////////////////////////////////////
    const TextFontEffectInterfacePtr & TTFFont::getEffect() const
    {
        return m_effect;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::initialize()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE, &TTFFont::notifyRenderDeviceLostPrepare, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::finalize()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );

        MENGINE_ASSERTION_FATAL( m_dataTTF == nullptr );

        this->clearGlyphs_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::isValid()
    {
        if( m_effect != nullptr )
        {
            if( m_effect->isValid() == false )
            {
                return false;
            }
        }

        DataflowInterfacePtr dataflowTTF = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

        if( dataflowTTF == nullptr )
        {
            return false;
        }

        const ContentInterface * content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        TTFDataInterfacePtr data = Helper::getDataflow( fileGroup, filePath, dataflowTTF, MENGINE_DOCUMENT_FACTORABLE );

        if( data == nullptr )
        {
            return false;
        }

        FT_Face face = data->getFTFace();

        GlyphCode code = 'A';

        FT_UInt glyph_index = FT_Get_Char_Index( face, code );

        if( FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR ) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_compile()
    {
        if( m_effect != nullptr )
        {
            if( m_effect->compile() == false )
            {
                return false;
            }
        }

        DataflowInterfacePtr dataflowTTF = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( dataflowTTF );

        const ContentInterface * content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        TTFDataInterfacePtr data = Helper::getDataflow( fileGroup, filePath, dataflowTTF, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data );

        m_dataTTF = data;

        FT_Face face;
        if( this->loadFaceGlyph_( 'A', &face ) == false )
        {
            return false;
        }

        FT_Size face_size = face->size;
        const FT_Size_Metrics & face_size_metrics = face_size->metrics;

        FT_Pos ascender = face_size_metrics.ascender >> 6;
        FT_Pos descender = face_size_metrics.descender >> 6;
        FT_Pos height = face_size_metrics.height >> 6;

        m_ttfAscender = static_cast<float>(ascender);
        m_ttfDescender = -static_cast<float>(descender);
        m_ttfHeight = static_cast<float>(height);

        float sampleInv = this->getSampleInv();

        m_ttfAscender *= sampleInv;
        m_ttfDescender *= sampleInv;
        m_ttfHeight *= sampleInv;

        m_ttfSpacing = m_ttfHeight - (m_ttfAscender + m_ttfDescender);

        FT_GlyphSlot face_glyphA = face->glyph;

        const FT_Glyph_Metrics & face_glyphA_metrics = face_glyphA->metrics;

        FT_Pos glyphA_bearingY = (face_glyphA_metrics.horiBearingY >> 6);

        m_ttfBearingYA = static_cast<float>(glyphA_bearingY) * sampleInv;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::_release()
    {
        if( m_effect != nullptr )
        {
            m_effect->release();
        }

        m_dataTTF = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        if( m_effect != nullptr )
        {
            if( m_effect->prefetch( _observer ) == false )
            {
                return false;
            }
        }

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( dataflow );

        const ContentInterface * content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        if( PREFETCHER_SERVICE()
            ->prefetchData( fileGroup, filePath, dataflow, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_unfetch()
    {
        if( m_effect != nullptr )
        {
            if( m_effect->unfetch() == false )
            {
                return false;
            }
        }

        const ContentInterface * content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        if( PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_prepareGlyph( GlyphCode _code, const DocumentPtr & _doc )
    {
        uint32_t code_hash = _code % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;
        VectorTTFGlyphs & glyphs = m_glyphsHash[code_hash];

        VectorTTFGlyphs::iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), [_code]( const TTFGlyph & _glyph )
        {
            return _glyph.ch == _code;
        } );

        if( it_found != glyphs.end() )
        {
            const TTFGlyph & g = *it_found;
            MENGINE_UNUSED( g );

            return true;
        }

        FT_Face face;
        if( this->loadFaceGlyph_( _code, &face ) == false )
        {
            return false;
        }

        FT_GlyphSlot glyph = face->glyph;

        const FT_Glyph_Metrics & metrics = glyph->metrics;

        FT_Pos glyph_dx = (metrics.horiBearingX >> 6);
        FT_Pos glyph_dy = (metrics.horiBearingY >> 6);
        FT_Pos glyph_w = (metrics.width >> 6);
        FT_Pos glyph_h = (metrics.height >> 6);

        float glyph_advance = static_cast<float>(metrics.horiAdvance >> 6);

        const FT_Bitmap & glyph_bitmap = glyph->bitmap;

        uint32_t bitmap_channel = 0;

        switch( glyph_bitmap.pixel_mode )
        {
        case FT_PIXEL_MODE_MONO:
            {
                bitmap_channel = 1;
            }break;
        case FT_PIXEL_MODE_GRAY:
            {
                bitmap_channel = 1;
            }break;
        case FT_PIXEL_MODE_GRAY2:
            {
                bitmap_channel = 2;
            }break;
        case FT_PIXEL_MODE_GRAY4:
            {
                bitmap_channel = 4;
            }break;
        case FT_PIXEL_MODE_BGRA:
            {
                bitmap_channel = 4;
            }break;
        case FT_PIXEL_MODE_NONE:
        case FT_PIXEL_MODE_LCD:
        case FT_PIXEL_MODE_LCD_V:
            {
                return false;
            }break;
        default:
            return false;
        };

        float sampleInv = this->getSampleInv();

        TTFGlyph ttf_glyph;
        ttf_glyph.ch = _code;
        ttf_glyph.advance = glyph_advance * sampleInv;

        if( glyph_bitmap.width == 0 || glyph_bitmap.rows == 0 )
        {
            for( uint32_t index = 0; index != m_ttfLayoutCount; ++index )
            {
                TTFGlyphQuad & quad = ttf_glyph.quads[index];

                quad.offset.x = 0.f;
                quad.offset.y = 0.f;
                quad.size.x = 0.f;
                quad.size.y = 0.f;

                mt::uv4_identity( quad.uv );
                quad.texture = nullptr;
            }

            glyphs.emplace_back( ttf_glyph );

            return true;
        }

        if( m_effect != nullptr )
        {
            m_effect->apply( glyph_bitmap.width, glyph_bitmap.rows, glyph_bitmap.pitch, glyph_bitmap.buffer, bitmap_channel, glyph->bitmap_left, glyph->bitmap_top, m_height, [&ttf_glyph, sampleInv, _doc]( uint32_t _index, int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, const void * _buffer, uint32_t _pitch, uint32_t _bytespp )
            {
                TTFFontTextureGlyphProvider provider( _width, _height, _buffer, _pitch, _bytespp );

                mt::uv4f uv;
                RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                    ->makeTextureGlyph( _width + 2, _height + 2, 1, _bytespp, &provider, &uv, _doc );

                MENGINE_ASSERTION_MEMORY_PANIC( texture );

                TTFGlyphQuad & quad = ttf_glyph.quads[_index];

                quad.offset.x = (float)_x * sampleInv;
                quad.offset.y = (float)_y * sampleInv;
                quad.size.x = (float)_width * sampleInv;
                quad.size.y = (float)_height * sampleInv;
                quad.uv = uv;
                quad.texture = texture;
            } );
        }
        else
        {
            TTFFontTextureGlyphProvider provider( glyph_bitmap.width, glyph_bitmap.rows, glyph_bitmap.buffer, glyph_bitmap.pitch, bitmap_channel );

            mt::uv4f uv;
            RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                ->makeTextureGlyph( glyph_bitmap.width + 2, glyph_bitmap.rows + 2, 1, bitmap_channel, &provider, &uv, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( texture );

            TTFGlyphQuad & quad = ttf_glyph.quads[0];

            quad.offset.x = (float)glyph_dx * sampleInv;
            quad.offset.y = (float)-glyph_dy * sampleInv;
            quad.size.x = (float)glyph_w * sampleInv;
            quad.size.y = (float)glyph_h * sampleInv;
            quad.uv = uv;
            quad.texture = texture;
        }

        glyphs.emplace_back( ttf_glyph );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_validateGlyphes( const U32String & _codes ) const
    {
        const ContentInterface * content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        MemoryInterfacePtr memory = Helper::createMemoryFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( memory == nullptr )
        {
            return false;
        }

        FT_Byte * memory_byte = memory->getBuffer();
        size_t memory_size = memory->getSize();

        if( memory_byte == nullptr )
        {
            return false;
        }

        FT_Face face;
        FT_Error err_code = FT_New_Memory_Face( m_ftlibrary, memory_byte, (FT_Long)memory_size, 0, &face );

        if( err_code != 0 )
        {
            return false;
        }

        if( FT_Select_Charmap( face, FT_ENCODING_UNICODE ) != FT_Err_Ok )
        {
            return false;
        }

        uint32_t sample = this->getSample();

        int32_t fe_height = (int32_t)(m_height * sample);

        if( FT_Set_Pixel_Sizes( face, 0, fe_height ) != FT_Err_Ok )
        {
            return false;
        }

        bool successful = true;

        for( U32String::const_iterator
            it = _codes.begin(),
            it_end = _codes.end();
            it != it_end;
            ++it )
        {
            FT_ULong ttf_code = (FT_ULong)*it;

            FT_UInt glyph_index = FT_Get_Char_Index( face, ttf_code );

            if( FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR ) )
            {
                LOGGER_ERROR( "ttf font '%s' not found glyph code '%lu'"
                    , this->getName().c_str()
                    , ttf_code
                );

                successful = false;

                continue;
            }

            FT_GlyphSlot glyph = face->glyph;

            FT_Bitmap bitmap = glyph->bitmap;

            uint8_t pixel_mode = bitmap.pixel_mode;

            switch( pixel_mode )
            {
            case FT_PIXEL_MODE_NONE:
            case FT_PIXEL_MODE_LCD:
            case FT_PIXEL_MODE_LCD_V:
                {
                    LOGGER_ERROR( "ttf font '%s' glyph '%lu' unsupport mode '%u'"
                        , this->getName().c_str()
                        , ttf_code
                        , pixel_mode
                    );

                    successful = false;
                }break;
            default:
                break;
            };
        }

        FT_Done_Face( face );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::hasGlyph( GlyphCode _code ) const
    {
        uint32_t code_hash = _code % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;
        const VectorTTFGlyphs & glyphs = m_glyphsHash[code_hash];

        VectorTTFGlyphs::const_iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), [_code]( const TTFGlyph & _glyph )
        {
            return _glyph.ch == _code;
        } );

        if( it_found == glyphs.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::getGlyph( uint32_t _layout, GlyphCode _code, GlyphCode _next, Glyph * const _glyph ) const
    {
        uint32_t code_hash = _code % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;
        const VectorTTFGlyphs & glyphs = m_glyphsHash[code_hash];

        VectorTTFGlyphs::const_iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), [_code]( const TTFGlyph & _glyph )
        {
            return _glyph.ch == _code;
        } );

        if( it_found == glyphs.end() )
        {
            return false;
        }

        const TTFGlyph & glyph = *it_found;

        _glyph->advance = glyph.advance;

        const TTFGlyphQuad & quad = glyph.quads[_layout];

        _glyph->offset = quad.offset;
        _glyph->size = quad.size;
        _glyph->uv = quad.uv;
        _glyph->texture = quad.texture;

        if( _next == 0 )
        {
            return true;
        }

        FT_Face face = m_dataTTF->getFTFace();

        FT_UInt rindex = FT_Get_Char_Index( face, _next );

        if( rindex == 0 )
        {
            return true;
        }

        FT_Vector ttf_kerning;
        FT_Get_Kerning( face, _code, _next, FT_KERNING_DEFAULT, &ttf_kerning );

        if( ttf_kerning.x == 0 )
        {
            return true;
        }

        float kerning = (float)(ttf_kerning.x >> 6);

        _glyph->advance += kerning;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TTFFont::getLayoutCount() const
    {
        return m_ttfLayoutCount;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontAscent() const
    {
        return m_ttfAscender;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontDescent() const
    {
        return m_ttfDescender;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontHeight() const
    {
        return m_ttfHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontBearingYA() const
    {
        return m_ttfBearingYA;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::getFontPremultiply() const
    {
        if( m_effect != nullptr )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontSpacing() const
    {
        return m_ttfSpacing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::loadFaceGlyph_( GlyphCode _code, FT_Face * const _face )
    {
        FT_Face face = m_dataTTF->getFTFace();

        uint32_t sample = this->getSample();

        int32_t fe_height = (int32_t)(m_height * sample);

        if( FT_Set_Pixel_Sizes( face, 0, fe_height ) != FT_Err_Ok )
        {
            LOGGER_ERROR( "font '%s' invalid set pixel height '%u'"
                , this->getName().c_str()
                , m_height
            );

            return false;
        }

        FT_UInt glyph_index = FT_Get_Char_Index( face, _code );

        if( FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR ) )
        {
            return false;
        }

        *_face = face;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::notifyRenderDeviceLostPrepare()
    {
        this->clearGlyphs_();
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::clearGlyphs_()
    {
        for( uint32_t index = 0; index != MENGINE_TTF_FONT_GLYPH_HASH_SIZE; ++index )
        {
            VectorTTFGlyphs & glyphs = m_glyphsHash[index];

            glyphs.clear();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TTFFont::getSample() const
    {
        if( m_effect == nullptr )
        {
            return 1;
        }

        uint32_t sample = m_effect->getEffectSample();

        return sample;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getSampleInv() const
    {
        if( m_effect == nullptr )
        {
            return 1.f;
        }

        float sample = m_effect->getEffectSampleInv();

        return sample;
    }
    //////////////////////////////////////////////////////////////////////////
}
