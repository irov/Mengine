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
#include "FEDataInterface.h"

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
        , m_system( false )
        , m_FESample( 1 )
        , m_ttfAscender( 0.f )
        , m_ttfDescender( 0.f )
        , m_ttfHeight( 0.f )
        , m_ttfBearingYA( 0.f )
        , m_ttfSpacing( 0.f )
        , m_ttfLayoutCount( 1 )
        , m_ttfFEEffect( nullptr )
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
    void TTFFont::setSystem( bool _system )
    {
        m_system = _system;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::getSystem() const
    {
        return m_system;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setTTFPath( const FilePath & _ttfPath )
    {
        m_ttfPath = _ttfPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & TTFFont::getTTFPath() const
    {
        return m_ttfPath;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setTTFFileGroup( const FileGroupInterfacePtr & _fileGroup )
    {
        m_ttfFileGroup = _fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & TTFFont::getTTFFileGroup() const
    {
        return m_ttfFileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setTTFFEPath( const FilePath & _ttfFEPath )
    {
        m_ttfFEPath = _ttfFEPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & TTFFont::getTTFFEPath() const
    {
        return m_ttfFEPath;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setTTFFEName( const ConstString & _ttfFEName )
    {
        m_ttfFEName = _ttfFEName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TTFFont::getTTFFEName() const
    {
        return m_ttfFEName;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setTTFFEFileGroup( const FileGroupInterfacePtr & _fileGroup )
    {
        m_ttfFEFileGroup = _fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & TTFFont::getTTFFEFileGroup() const
    {
        return m_ttfFEFileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setFESample( uint32_t _FESample )
    {
        m_FESample = _FESample;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TTFFont::getFESample() const
    {
        return m_FESample;
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

        m_dataTTF = nullptr;
        m_ttfFileGroup = nullptr;

        m_ttfFEFileGroup = nullptr;
        m_dataFE = nullptr;

        this->clearGlyphs_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::isValid()
    {
        if( m_ttfFEPath.empty() == false )
        {
            DataflowInterfacePtr dataflowFE = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );

            FEDataInterfacePtr data = Helper::getDataflow( m_ttfFEFileGroup, m_ttfFEPath, dataflowFE, MENGINE_DOCUMENT_FACTORABLE );

            if( data == nullptr )
            {
                return false;
            }

            fe_bundle * ttfFEBundle = data->getFEBundle();

            if( ttfFEBundle == nullptr )
            {
                return false;
            }

            fe_effect * ttfFEEffect = fe_bundle_get_effect_by_name( ttfFEBundle, m_ttfFEName.c_str() );

            if( ttfFEEffect == nullptr )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_compile()
    {
        if( m_ttfFEPath.empty() == false )
        {
            DataflowInterfacePtr dataflowFE = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );

            FEDataInterfacePtr data = Helper::getDataflow( m_ttfFEFileGroup, m_ttfFEPath, dataflowFE, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( data );

            m_dataFE = data;

            fe_bundle * ttfFEBundle = m_dataFE->getFEBundle();

            fe_effect * ttfFEEffect = fe_bundle_get_effect_by_name( ttfFEBundle, m_ttfFEName.c_str() );

            MENGINE_ASSERTION_MEMORY_PANIC( ttfFEEffect );

            m_ttfFEEffect = ttfFEEffect;

            fe_node * effect_node_out = fe_effect_find_node_by_type( m_ttfFEEffect, fe_node_type_out );

            if( effect_node_out->properties_int[0] != 0 )
            {
                uint32_t layoutCount = 0;

                for( int32_t i = 0; i < FE_MAX_PINS; ++i )
                {
                    const fe_node * effect_node_layout = effect_node_out->in[FE_MAX_PINS - i - 1].node;

                    if( effect_node_layout != nullptr )
                    {
                        m_ttfEffectNodes[layoutCount] = effect_node_layout;
                        ++layoutCount;
                    }
                }

                m_ttfLayoutCount = layoutCount;
            }
            else
            {
                m_ttfEffectNodes[0] = effect_node_out;

                m_ttfLayoutCount = 1;
            }
        }

        DataflowInterfacePtr dataflowTTF = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

        TTFDataInterfacePtr data = Helper::getDataflow( m_ttfFileGroup, m_ttfPath, dataflowTTF, MENGINE_DOCUMENT_FACTORABLE );

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

        if( m_FESample == 2 )
        {
            m_ttfAscender *= 0.5f;
            m_ttfDescender *= 0.5f;
            m_ttfHeight *= 0.5f;
        }

        m_ttfSpacing = m_ttfHeight - (m_ttfAscender + m_ttfDescender);

        FT_GlyphSlot face_glyphA = face->glyph;

        const FT_Glyph_Metrics & face_glyphA_metrics = face_glyphA->metrics;

        FT_Pos glyphA_bearingY = (face_glyphA_metrics.horiBearingY >> 6);

        m_ttfBearingYA = static_cast<float>(glyphA_bearingY);

        if( m_FESample == 2 )
        {
            m_ttfBearingYA *= 0.5f;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::_release()
    {
        m_dataTTF = nullptr;
        m_dataFE = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        if( m_ttfFEPath.empty() == false )
        {
            DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( dataflow );

            if( PREFETCHER_SERVICE()
                ->prefetchData( m_ttfFEFileGroup, m_ttfFEPath, dataflow, _observer ) == false )
            {
                return false;
            }
        }

        {
            DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( dataflow );

            if( PREFETCHER_SERVICE()
                ->prefetchData( m_ttfFileGroup, m_ttfPath, dataflow, _observer ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_unfetch()
    {
        if( m_ttfFEPath.empty() == false )
        {
            if( PREFETCHER_SERVICE()
                ->unfetch( m_ttfFEFileGroup, m_ttfFEPath ) == false )
            {
                return false;
            }
        }

        if( PREFETCHER_SERVICE()
            ->unfetch( m_ttfFileGroup, m_ttfPath ) == false )
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

        FT_Bitmap glyph_bitmap = glyph->bitmap;

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

        TTFGlyph ttf_glyph;
        ttf_glyph.ch = _code;
        ttf_glyph.advance = glyph_advance / m_FESample;

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

        if( m_ttfFEEffect != nullptr )
        {
            int32_t im_image_w = glyph_bitmap.width;
            int32_t im_image_h = glyph_bitmap.rows;
            int32_t im_image_pitch = glyph_bitmap.pitch;
            const void * im_image_data = glyph_bitmap.buffer;

            FE_IMAGE_FORMAT im_image_format = TF_UNDEFINED;
            switch( bitmap_channel )
            {
            case 1:
                im_image_format = FE_IMG_A8;
                break;
            case 4:
                im_image_format = FE_IMG_R8G8B8A8;
                break;
            default:
                break;
            }

            FT_Int im_x = glyph->bitmap_left;
            FT_Int im_y = glyph->bitmap_top;

            int32_t max_bgra_width = 0U;
            int32_t total_bgra_height = 0U;

            fe_im bgra_res[FE_MAX_PINS];
            fe_image bgra_images[FE_MAX_PINS];
            for( uint32_t layoutIndex = 0; layoutIndex != m_ttfLayoutCount; ++layoutIndex )
            {
                const fe_node * effect_node_layout = m_ttfEffectNodes[layoutIndex];

                fe_im & res = bgra_res[layoutIndex];
                fe_image & bgra_image = bgra_images[layoutIndex];

                int fe_height = (int)(m_height * m_FESample);

                fe_node_apply( fe_height
                    , im_x
                    , im_y
                    , im_image_w
                    , im_image_h
                    , im_image_format
                    , im_image_pitch
                    , im_image_data
                    , effect_node_layout, &res );

#ifdef MENGINE_RENDER_TEXTURE_RGBA
                fe_image_create( &bgra_image, res.image.w, res.image.h, FE_IMG_R8G8B8A8 );
#else
                fe_image_create( &bgra_image, res.image.w, res.image.h, FE_IMG_B8G8R8A8 );
#endif

                fe_image_blit( &res.image, &bgra_image );

                fe_image_free( &res.image );

                if( bgra_image.w + 2 > max_bgra_width )
                {
                    max_bgra_width = bgra_image.w + 2;
                }

                total_bgra_height += bgra_image.h + 2;
            }

            uint32_t bgra_image_offset = 0U;
            for( uint32_t layoutIndex = 0; layoutIndex != m_ttfLayoutCount; ++layoutIndex )
            {
                const fe_im & res = bgra_res[layoutIndex];
                fe_image & bgra_image = bgra_images[layoutIndex];

                switch( m_FESample )
                {
                case 1:
                    {
                        TTFFontTextureGlyphProvider provider( bgra_image.w, bgra_image.h, bgra_image.data, bgra_image.pitch, bgra_image.bytespp );

                        mt::uv4f uv;
                        RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                            ->makeTextureGlyph( bgra_image.w + 2, bgra_image.h + 2, 1, bgra_image.bytespp, &provider, &uv, _doc );

                        fe_image_free( &bgra_image );

                        MENGINE_ASSERTION_MEMORY_PANIC( texture );

                        TTFGlyphQuad & quad = ttf_glyph.quads[layoutIndex];

                        quad.offset.x = (float)res.x;
                        quad.offset.y = (float)-res.y;
                        quad.size.x = (float)bgra_image.w;
                        quad.size.y = (float)bgra_image.h;
                        quad.uv = uv;
                        quad.texture = texture;
                    }break;
                case 2:
                    {
                        TTFFontTextureGlyphProvider provider( bgra_image.w, bgra_image.h, bgra_image.data, bgra_image.pitch, bgra_image.bytespp );

                        mt::uv4f uv;
                        RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                            ->makeTextureGlyph( bgra_image.w + 2, bgra_image.h + 2, 1, bgra_image.bytespp, &provider, &uv, _doc );

                        fe_image_free( &bgra_image );

                        MENGINE_ASSERTION_MEMORY_PANIC( texture );

                        TTFGlyphQuad & quad = ttf_glyph.quads[layoutIndex];

                        quad.offset.x = (float)res.x * 0.5f;
                        quad.offset.y = (float)-res.y * 0.5f;
                        quad.size.x = (float)bgra_image.w * 0.5f;
                        quad.size.y = (float)bgra_image.h * 0.5f;
                        quad.uv = uv;
                        quad.texture = texture;
                    }break;
                default:
                    return false;
                }

                bgra_image_offset += bgra_image.h + 2;
            }
        }
        else
        {
            TTFFontTextureGlyphProvider provider( glyph_bitmap.width, glyph_bitmap.rows, glyph_bitmap.buffer, glyph_bitmap.pitch, bitmap_channel );

            mt::uv4f uv;
            RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                ->makeTextureGlyph( glyph_bitmap.width + 2, glyph_bitmap.rows + 2, 1, bitmap_channel, &provider, &uv, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( texture );

            TTFGlyphQuad & quad = ttf_glyph.quads[0];

            quad.offset.x = (float)glyph_dx;
            quad.offset.y = (float)-glyph_dy;
            quad.size.x = (float)glyph_w;
            quad.size.y = (float)glyph_h;
            quad.uv = uv;
            quad.texture = texture;
        }

        glyphs.emplace_back( ttf_glyph );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_validateGlyphes( const U32String & _codes ) const
    {
        MemoryInterfacePtr memory = Helper::createMemoryFile( m_ttfFileGroup, m_ttfPath, false, false, MENGINE_DOCUMENT_FACTORABLE );

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

        int fe_height = (int)(m_height * m_FESample);

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
            FT_ULong ttf_code = (FT_ULong)* it;

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
        if( m_ttfFEEffect != nullptr )
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

        int fe_height = (int)(m_height * m_FESample);

        if( FT_Set_Pixel_Sizes( face, 0, fe_height ) != FT_Err_Ok )
        {
            LOGGER_ERROR( "font '%s' invalid set pixel height '%u'"
                , m_name.c_str()
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
}
