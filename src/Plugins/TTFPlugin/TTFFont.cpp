#include "TTFFont.h"

#include "TTFInterface.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "TTFDataInterface.h"
#include "TTFFontTextureGlyphProvider.h"

#include "Kernel/Dataflow.h"
#include "Kernel/StreamHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdAlgorithm.h"

#define MENGINE_FT_FLOOR(X) (((X) & -64) / 64)
#define MENGINE_FT_CEIL(X) MENGINE_FT_FLOOR((X) + 63)
#define MENGINE_FT_CEILF(X) ((float)MENGINE_FT_CEIL((X)))
#define MENGINE_FT_INV64 (1.f / 64.f)

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static MemoryInterfacePtr normalizeBitmap( const FT_Bitmap & _bitmap, uint32_t _channel, bool _rgba, const DocumentInterfacePtr & _doc )
        {
            size_t pitch = (size_t)_bitmap.width * _channel;
            size_t size = pitch * _bitmap.rows;

            MemoryInterfacePtr memory = Helper::createMemoryCacheBuffer( size, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid allocate font bitmap memory %zu", size );

            uint8_t * buffer = memory->getBuffer();

            uint32_t depth = 8;

            switch( _bitmap.pixel_mode )
            {
            case FT_PIXEL_MODE_MONO:
                {
                    depth = 1;
                }break;
            case FT_PIXEL_MODE_GRAY2:
                {
                    depth = 2;
                }break;
            case FT_PIXEL_MODE_GRAY4:
                {
                    depth = 4;
                }break;
            default:
                break;
            }

            uint32_t maxValue = (1U << depth) - 1U;

            if( _bitmap.pixel_mode == FT_PIXEL_MODE_GRAY && _bitmap.num_grays > 1 )
            {
                maxValue = _bitmap.num_grays - 1U;
            }

            const uint8_t * origin = _bitmap.buffer;

            if( _bitmap.pitch < 0 )
            {
                origin -= (ptrdiff_t)_bitmap.pitch * (ptrdiff_t)(_bitmap.rows - 1);
            }

            for( uint32_t y = 0; y != _bitmap.rows; ++y )
            {
                const uint8_t * source = origin + (ptrdiff_t)y * _bitmap.pitch;
                uint8_t * target = buffer + y * pitch;

                for( uint32_t x = 0; x != _bitmap.width; ++x )
                {
                    if( _channel == 1 )
                    {
                        uint32_t bit = x * depth;
                        uint32_t shift = 8U - depth - (bit % 8U);
                        uint32_t value = (source[bit / 8U] >> shift) & ((1U << depth) - 1U);

                        target[x] = (uint8_t)(value * 255U / maxValue);

                        continue;
                    }

                    uint32_t alpha = source[x * 4U + 3U];
                    uint32_t red = 0;
                    uint32_t green = 0;
                    uint32_t blue = 0;

                    if( alpha != 0 )
                    {
                        red = MENGINE_MIN( 255U, (source[x * 4U + 2U] * 255U + alpha / 2U) / alpha );
                        green = MENGINE_MIN( 255U, (source[x * 4U + 1U] * 255U + alpha / 2U) / alpha );
                        blue = MENGINE_MIN( 255U, (source[x * 4U] * 255U + alpha / 2U) / alpha );
                    }

                    target[x * 4U] = (uint8_t)(_rgba == true ? red : blue);
                    target[x * 4U + 1U] = (uint8_t)green;
                    target[x * 4U + 2U] = (uint8_t)(_rgba == true ? blue : red);
                    target[x * 4U + 3U] = (uint8_t)alpha;
                }
            }

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool shapeText( hb_font_t * _font, const U32String & _text, const Char * _language, float _scale, VectorFontShapedGlyphs * const _glyphs )
        {
            hb_buffer_t * buffer = hb_buffer_create();
            hb_unicode_funcs_t * unicode = hb_unicode_funcs_get_default();
            hb_language_t language = hb_language_from_string( _language, -1 );

            bool successful = true;
            size_t begin = 0;
            size_t size = _text.size();

            while( begin != size )
            {
                hb_script_t script = HB_SCRIPT_UNKNOWN;
                size_t end = begin;

                for( ; end != size; ++end )
                {
                    hb_script_t next = hb_unicode_script( unicode, _text[end] );

                    if( next == HB_SCRIPT_COMMON || next == HB_SCRIPT_INHERITED || next == HB_SCRIPT_UNKNOWN )
                    {
                        continue;
                    }

                    if( script == HB_SCRIPT_UNKNOWN )
                    {
                        script = next;

                        continue;
                    }

                    if( next != script )
                    {
                        break;
                    }
                }

                MENGINE_ASSERTION_FATAL( hb_script_get_horizontal_direction( script ) != HB_DIRECTION_RTL, "shaping language '%s' got right-to-left text, only left-to-right is supported"
                    , _language
                );

                hb_buffer_reset( buffer );
                hb_buffer_set_direction( buffer, HB_DIRECTION_LTR );
                hb_buffer_set_script( buffer, script );
                hb_buffer_set_language( buffer, language );
                hb_buffer_set_cluster_level( buffer, HB_BUFFER_CLUSTER_LEVEL_MONOTONE_GRAPHEMES );

                for( size_t index = begin; index != end; ++index )
                {
                    Char32 code = _text[index];
                    uint32_t cluster = (uint32_t)index;

                    hb_buffer_add( buffer, code, cluster );
                }

                hb_buffer_set_content_type( buffer, HB_BUFFER_CONTENT_TYPE_UNICODE );

                if( hb_shape_full( _font, buffer, nullptr, 0, nullptr ) == false )
                {
                    successful = false;

                    break;
                }

                unsigned int count;
                const hb_glyph_info_t * infos = hb_buffer_get_glyph_infos( buffer, &count );
                const hb_glyph_position_t * positions = hb_buffer_get_glyph_positions( buffer, nullptr );

                for( unsigned int index = 0; index != count; ++index )
                {
                    const hb_glyph_info_t & info = infos[index];
                    const hb_glyph_position_t & position = positions[index];

                    FontShapedGlyphDesc glyph;
                    glyph.index = info.codepoint;
                    glyph.cluster = info.cluster;
                    glyph.unsafeToBreak = (hb_glyph_info_get_glyph_flags( &info ) & HB_GLYPH_FLAG_UNSAFE_TO_BREAK) != 0;
                    glyph.advance = (float)position.x_advance * MENGINE_FT_INV64 * _scale;
                    glyph.offset.x = (float)position.x_offset * MENGINE_FT_INV64 * _scale;
                    glyph.offset.y = -(float)position.y_offset * MENGINE_FT_INV64 * _scale;

                    _glyphs->emplace_back( glyph );
                }

                begin = end;
            }

            hb_buffer_destroy( buffer );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    TTFFont::TTFFont()
        : m_ftlibrary( nullptr )
        , m_hbFont( nullptr )
        , m_ttfAscender( 0.f )
        , m_ttfDescender( 0.f )
        , m_ttfHeight( 0.f )
        , m_ttfBearingYA( 0.f )
        , m_ttfSpacing( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFFont::~TTFFont()
    {
        MENGINE_ASSERTION_OBSERVABLE( this, "ttf font '%s'"
            , this->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setFTLibrary( FT_Library _ftlibrary )
    {
        m_ftlibrary = _ftlibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    FT_Library TTFFont::getFTLibrary() const
    {
        return m_ftlibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setEffect( const FontEffectInterfacePtr & _effect )
    {
        if( m_effect == _effect )
        {
            return;
        }

        MENGINE_ASSERTION_FATAL( this->isCompile() == false, "font '%s' set effect while compiled"
            , this->getName().c_str()
        );

        m_effect = _effect;

        this->clearGlyphs_();
    }
    //////////////////////////////////////////////////////////////////////////
    const FontEffectInterfacePtr & TTFFont::getEffect() const
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
        MENGINE_ASSERTION_FATAL( this->isCompile() == false, "font '%s' not stop compile"
            , this->getName().c_str()
        );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );

        m_glyph = nullptr;

        this->clearGlyphs_();

        m_effect = nullptr;
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

        if( m_glyph == nullptr )
        {
            LOGGER_ERROR( "ttf font '%s' not setup glyph"
                , this->getName().c_str()
            );

            return false;
        }

        TTFFontGlyph * glyph = m_glyph.getT<TTFFontGlyph *>();

        if( glyph->compile() == false )
        {
            return false;
        }

        FT_Face face;
        if( this->loadFaceGlyph_( MENGINE_TTF_METRICS_SYMBOL, &face ) == false )
        {
            return false;
        }

        FT_Size face_size = face->size;

        const FT_Size_Metrics & face_size_metrics = face_size->metrics;

        FT_Fixed scale = face_size_metrics.y_scale;

        m_ttfAscender = MENGINE_FT_CEILF( FT_MulFix( face->ascender, scale ) );
        m_ttfDescender = MENGINE_FT_CEILF( FT_MulFix( face->descender, scale ) );
        m_ttfHeight = MENGINE_FT_CEILF( FT_MulFix( face->ascender - face->descender, scale ) );

        m_ttfSpacing = m_ttfHeight - (m_ttfAscender - m_ttfDescender);

        FT_GlyphSlot face_glyphA = face->glyph;

        const FT_Glyph_Metrics & face_glyphA_metrics = face_glyphA->metrics;

        float glyphA_bearingY = (float)face_glyphA_metrics.horiBearingY * MENGINE_FT_INV64;

        m_ttfBearingYA = glyphA_bearingY;

        float sampleInv = this->getSampleInv();

        m_ttfAscender *= sampleInv;
        m_ttfDescender *= sampleInv;
        m_ttfHeight *= sampleInv;
        m_ttfSpacing *= sampleInv;
        m_ttfBearingYA *= sampleInv;

        if( this->isTextShaping() == true )
        {
            hb_font_t * hbFont = hb_ft_font_create_referenced( face );
            hb_ft_font_set_load_flags( hbFont, FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR );

            m_hbFont = hbFont;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::_release()
    {
        if( m_hbFont != nullptr )
        {
            hb_font_destroy( m_hbFont );
            m_hbFont = nullptr;
        }

        if( m_effect != nullptr )
        {
            m_effect->release();
        }

        MENGINE_ASSERTION_FATAL( m_glyph != nullptr, "ttf font '%s' release after finalize (glyph is null)"
            , this->getName().c_str()
        );

        TTFFontGlyph * glyph = m_glyph.getT<TTFFontGlyph *>();

        glyph->release();
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::_unfetch()
    {
        if( m_effect != nullptr )
        {
            m_effect->unfetch();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_prepareGlyph( GlyphCode _code, const DocumentInterfacePtr & _doc )
    {
        MENGINE_PROFILER_CATEGORY();

        const TTFFontGlyph * fontGlyph = m_glyph.getT<const TTFFontGlyph *>();
        FT_Face face = this->getFTFace( fontGlyph->getTTFData() );

        if( face == nullptr )
        {
            return false;
        }

        FT_UInt index = FT_Get_Char_Index( face, _code );

        return this->prepareGlyphIndex_( face, index, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::prepareGlyphIndex_( FT_Face _face, uint32_t _index, const DocumentInterfacePtr & _doc )
    {
        uint32_t code_hash = _index % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;
        VectorTTFGlyphs & glyphs = m_glyphsHash[code_hash];

        VectorTTFGlyphs::const_iterator it_found = StdAlgorithm::find_if( glyphs.begin(), glyphs.end(), [_index]( const TTFGlyph & _glyph )
        {
            return _glyph.index == _index;
        } );

        if( it_found != glyphs.end() )
        {
            return true;
        }

        FT_Error error = FT_Load_Glyph( _face, _index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR );

        if( error != FT_Err_Ok )
        {
            LOGGER_ERROR( "ttf font '%s' invalid load glyph index %u error [%s]"
                , this->getName().c_str()
                , _index
                , FT_Error_String( error )
            );

            return false;
        }

        FT_GlyphSlot glyph = _face->glyph;

        const FT_Glyph_Metrics & metrics = glyph->metrics;

        float glyph_advance = (float)metrics.horiAdvance * MENGINE_FT_INV64;

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
                bitmap_channel = 1;
            }break;
        case FT_PIXEL_MODE_GRAY4:
            {
                bitmap_channel = 1;
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
        ttf_glyph.index = _index;
        ttf_glyph.advance = glyph_advance * sampleInv;

        if( glyph_bitmap.width == 0 || glyph_bitmap.rows == 0 )
        {
            uint32_t layoutCount = this->getLayoutCount();

            MENGINE_ASSERTION_FATAL( layoutCount <= MENGINE_TTF_FONT_MAX_LAYOUTS, "font '%s' invalid layout count %u max %u"
                , this->getName().c_str()
                , layoutCount
                , MENGINE_TTF_FONT_MAX_LAYOUTS
            );

            for( uint32_t index = 0; index != layoutCount; ++index )
            {
                TTFGlyphQuad & quad = ttf_glyph.quads[index];

                quad.offset.x = 0.f;
                quad.offset.y = 0.f;
                quad.size.x = 0.f;
                quad.size.y = 0.f;

                mt::uv4_identity( &quad.uv );
                quad.texture = nullptr;
            }

            glyphs.emplace_back( ttf_glyph );

            return true;
        }

        const uint8_t * bitmapBuffer = glyph_bitmap.buffer;
        ptrdiff_t bitmapPitch = glyph_bitmap.pitch;
        MemoryInterfacePtr bitmapMemory;

        if( glyph_bitmap.pixel_mode != FT_PIXEL_MODE_GRAY || glyph_bitmap.num_grays != 256 || glyph_bitmap.pitch < 0 )
        {
#if defined(MENGINE_RENDER_TEXTURE_RGBA)
            bool rgba = true;
#else
            bool rgba = (m_effect != nullptr);
#endif

            bitmapMemory = Detail::normalizeBitmap( glyph_bitmap, bitmap_channel, rgba, _doc );
            bitmapBuffer = bitmapMemory->getBuffer();
            bitmapPitch = (ptrdiff_t)glyph_bitmap.width * bitmap_channel;
        }

        uint32_t texture_border = 1;

        if( m_effect != nullptr )
        {
            m_effect->apply( glyph_bitmap.width, glyph_bitmap.rows, (uint32_t)bitmapPitch, bitmapBuffer, bitmap_channel, glyph->bitmap_left, glyph->bitmap_top, m_height, [texture_border, &ttf_glyph, sampleInv, _doc]( uint32_t _index, int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, const void * _buffer, uint32_t _pitch, uint32_t _bytespp )
            {
                MENGINE_ASSERTION_FATAL( _index < MENGINE_TTF_FONT_MAX_LAYOUTS, "invalid font effect layout index %u max %u"
                    , _index
                    , MENGINE_TTF_FONT_MAX_LAYOUTS
                );

                TTFFontTextureGlyphProvider provider( _width, _height, _buffer, _pitch, _bytespp );

                mt::uv4f uv;
                RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                    ->makeTextureGlyph( _width, _height, texture_border, _bytespp, &provider, &uv, _doc );

                MENGINE_ASSERTION_MEMORY_PANIC( texture, "invalid make texture glyph" );

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
            TTFFontTextureGlyphProvider provider( glyph_bitmap.width, glyph_bitmap.rows, bitmapBuffer, bitmapPitch, bitmap_channel );

            mt::uv4f uv;
            RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                ->makeTextureGlyph( glyph_bitmap.width, glyph_bitmap.rows, texture_border, bitmap_channel, &provider, &uv, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( texture, "invalid make texture glyph" );

            TTFGlyphQuad & quad = ttf_glyph.quads[0];

            quad.offset.x = (float)glyph->bitmap_left * sampleInv;
            quad.offset.y = -(float)glyph->bitmap_top * sampleInv;
            quad.size.x = (float)glyph_bitmap.width * sampleInv;
            quad.size.y = (float)glyph_bitmap.rows * sampleInv;
            quad.uv = uv;
            quad.texture = texture;
        }

        glyphs.emplace_back( ttf_glyph );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_validateGlyphes( const U32String & _codes ) const
    {
        const TTFFontGlyph * ttfGlyph = m_glyph.getT<const TTFFontGlyph *>();

        const ContentInterfacePtr & content = ttfGlyph->getGlyphContent();

        MemoryInterfacePtr memory = content->createMemoryFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

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
            const Char * err_message = FT_Error_String( err_code );

            LOGGER_ERROR( "ttf font '%s' invalid new memory face size '%zu' error [%s]"
                , this->getName().c_str()
                , memory_size
                , err_message
            );

            return false;
        }

        err_code = FT_Select_Charmap( face, FT_ENCODING_UNICODE );

        if( err_code != FT_Err_Ok )
        {
            const Char * err_message = FT_Error_String( err_code );

            LOGGER_ERROR( "ttf font '%s' invalid select charmap 'FT_ENCODING_UNICODE' error [%s]"
                , this->getName().c_str()
                , err_message
            );

            FT_Done_Face( face );

            return false;
        }

        if( this->updateFTFaceSize( face ) == false )
        {
            FT_Done_Face( face );

            return false;
        }

        bool successful = true;

        for( Char32 ch : _codes )
        {
            FT_ULong ttf_code = (FT_ULong)ch;

            FT_UInt glyph_index = FT_Get_Char_Index( face, ttf_code );

            err_code = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR );

            if( err_code != FT_Err_Ok )
            {
                const Char * err_message = FT_Error_String( err_code );

                LOGGER_ERROR( "ttf font '%s' invalid load glyph code '%lu' error [%s]"
                    , this->getName().c_str()
                    , ttf_code
                    , err_message
                );

                successful = false;

                continue;
            }

            FT_GlyphSlot glyph = face->glyph;

            const FT_Bitmap & bitmap = glyph->bitmap;

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
    bool TTFFont::loadFaceGlyph_( GlyphCode _code, FT_Face * const _face ) const
    {
        const TTFFontGlyph * fontGlyph = m_glyph.getT<const TTFFontGlyph *>();

        const TTFDataInterfacePtr dataTTF = fontGlyph->getTTFData();

        FT_Face face = this->getFTFace( dataTTF );

        FT_UInt glyph_index = FT_Get_Char_Index( face, _code );

        FT_Error err_code = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR );

        if( err_code != FT_Err_Ok )
        {
            const Char * err_message = FT_Error_String( err_code );

            LOGGER_ERROR( "ttf font '%s' invalid load glyph code '%u' error [%s]"
                , this->getName().c_str()
                , _code
                , err_message
            );

            return false;
        }

        *_face = face;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setShapingLanguage( const ConstString & _language )
    {
        m_shapingLanguage = _language;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TTFFont::getShapingLanguage() const
    {
        return m_shapingLanguage;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::isTextShaping() const
    {
        return m_shapingLanguage.empty() == false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::shapeText( const U32String & _text, VectorFontShapedGlyphs * const _glyphs, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_hbFont, "ttf font '%s' shape text without shaping font"
            , this->getName().c_str()
        );

        const TTFFontGlyph * fontGlyph = m_glyph.getT<const TTFFontGlyph *>();
        FT_Face face = this->getFTFace( fontGlyph->getTTFData() );

        if( face == nullptr )
        {
            return false;
        }

        hb_ft_font_changed( m_hbFont );

        const Char * language = m_shapingLanguage.c_str();
        float sampleInv = this->getSampleInv();

        if( Detail::shapeText( m_hbFont, _text, language, sampleInv, _glyphs ) == false )
        {
            return false;
        }

        for( const FontShapedGlyphDesc & glyph : *_glyphs )
        {
            if( this->prepareGlyphIndex_( face, glyph.index, _doc ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::getShapedGlyph( uint32_t _layout, uint32_t _index, Glyph * const _glyph ) const
    {
        if( this->getGlyphIndex_( _layout, _index, _glyph ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::hasGlyph( GlyphCode _code ) const
    {
        const TTFFontGlyph * fontGlyph = m_glyph.getT<const TTFFontGlyph *>();
        FT_Face face = this->getFTFace( fontGlyph->getTTFData() );

        if( face == nullptr )
        {
            return false;
        }

        FT_UInt index = FT_Get_Char_Index( face, _code == 160 ? 32 : _code );
        Glyph glyph;
        return this->getGlyphIndex_( 0, index, &glyph );
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::getGlyphIndex_( uint32_t _layout, uint32_t _index, Glyph * const _glyph ) const
    {
        uint32_t code_hash = _index % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;
        const VectorTTFGlyphs & glyphs = m_glyphsHash[code_hash];

        VectorTTFGlyphs::const_iterator it_found = StdAlgorithm::find_if( glyphs.begin(), glyphs.end(), [_index]( const TTFGlyph & _glyph )
        {
            return _glyph.index == _index;
        } );

        if( it_found == glyphs.end() )
        {
            return false;
        }

        const TTFGlyph & glyph = *it_found;
        const TTFGlyphQuad & quad = glyph.quads[_layout];

        _glyph->advance = glyph.advance;
        _glyph->offset = quad.offset;
        _glyph->size = quad.size;
        _glyph->uv = quad.uv;
        _glyph->texture = quad.texture;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::getGlyph( uint32_t _layout, GlyphCode _code, GlyphCode _next, Glyph * const _glyph ) const
    {
        const TTFFontGlyph * fontGlyph = m_glyph.getT<const TTFFontGlyph *>();
        FT_Face face = this->getFTFace( fontGlyph->getTTFData() );

        if( face == nullptr )
        {
            return false;
        }

        _code = _code == 160 ? 32 : _code;
        _next = _next == 160 ? 32 : _next;
        FT_UInt index = FT_Get_Char_Index( face, _code );

        if( this->getGlyphIndex_( _layout, index, _glyph ) == false )
        {
            return false;
        }

        if( _next == 0 )
        {
            return true;
        }

        if( FT_HAS_KERNING( face ) == false )
        {
            return true;
        }

        FT_UInt code_index = FT_Get_Char_Index( face, _code );
        FT_UInt next_index = FT_Get_Char_Index( face, _next );

        if( next_index == 0 )
        {
            return true;
        }

        FT_Vector ttf_kerning;
        FT_Error err_kerning_code = FT_Get_Kerning( face, code_index, next_index, FT_KERNING_DEFAULT, &ttf_kerning );

        if( err_kerning_code != FT_Err_Ok )
        {
            const Char * err_message = FT_Error_String( err_kerning_code );

            LOGGER_ERROR( "ttf font '%s' invalid get kerning code '%u' next '%u' error [%s]"
                , this->getName().c_str()
                , _code
                , _next
                , err_message
            );

            return true;
        }

        if( ttf_kerning.x == 0 )
        {
            return true;
        }

        float sampleInv = this->getSampleInv();
        float kerning = (float)ttf_kerning.x * MENGINE_FT_INV64 * sampleInv;

        _glyph->advance += kerning;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TTFFont::getLayoutCount() const
    {
        if( m_effect != nullptr )
        {
            uint32_t layoutCount = m_effect->getLayoutCount();

            return layoutCount;
        }

        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontAscender() const
    {
        return m_ttfAscender;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontDescender() const
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
        if( m_effect != nullptr )
        {
            uint32_t sample = m_effect->getEffectSample();

            return sample;
        }

        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getSampleInv() const
    {
        if( m_effect != nullptr )
        {
            float sample = m_effect->getEffectSampleInv();

            return sample;
        }

        return 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    FT_Face TTFFont::getFTFace( const TTFDataInterfacePtr & _data ) const
    {
        FT_Face face = _data->getFTFace();

        if( this->updateFTFaceSize( face ) == false )
        {
            return nullptr;
        }

        return face;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::updateFTFaceSize( FT_Face _face ) const
    {
        uint32_t sample = this->getSample();

        int32_t fe_height = (int32_t)(m_height * sample);

        FT_Error err_pixel_sizes = FT_Set_Pixel_Sizes( _face, 0, fe_height );

        if( err_pixel_sizes != FT_Err_Ok )
        {
            const Char * err_message = FT_Error_String( err_pixel_sizes );

            LOGGER_ERROR( "ttf font '%s' invalid set pixel sizes '%u' error [%s]"
                , this->getName().c_str()
                , fe_height
                , err_message
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
