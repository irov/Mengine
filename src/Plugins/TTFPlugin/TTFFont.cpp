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
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/Algorithm.h"

#define MENGINE_FT_FLOOR(X) (((X) & -64) / 64)
#define MENGINE_FT_CEIL(X) MENGINE_FT_FLOOR((X) + 63)
#define MENGINE_FT_CEILF(X) ((float)MENGINE_FT_CEIL((X)))
#define MENGINE_FT_INV64 (1.f / 64.f)

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
    void TTFFont::setTTFFontGlyph( const TTFFontGlyphPtr & _glyph )
    {
        m_glyph = _glyph;
    }
    //////////////////////////////////////////////////////////////////////////
    const TTFFontGlyphPtr & TTFFont::getTTFFontGlyph() const
    {
        return m_glyph;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::setEffect( const FontEffectInterfacePtr & _effect )
    {
        m_effect = _effect;
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

        if( m_glyph->compile() == false )
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFont::_release()
    {
        if( m_effect != nullptr )
        {
            m_effect->release();
        }

        m_glyph->release();
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

        uint32_t code_hash = _code % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;
        VectorTTFGlyphs & glyphs = m_glyphsHash[code_hash];

        VectorTTFGlyphs::const_iterator it_found = Algorithm::find_if( glyphs.begin(), glyphs.end(), [_code]( const TTFGlyph & _glyph )
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

        float glyph_dx = (float)metrics.horiBearingX * MENGINE_FT_INV64;
        float glyph_dy = (float)metrics.horiBearingY * MENGINE_FT_INV64;
        float glyph_w = (float)metrics.width * MENGINE_FT_INV64;
        float glyph_h = (float)metrics.height * MENGINE_FT_INV64;

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
            uint32_t layoutCount = this->getLayoutCount();

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

        uint32_t texture_border = 1;

        if( m_effect != nullptr )
        {
            m_effect->apply( glyph_bitmap.width, glyph_bitmap.rows, glyph_bitmap.pitch, glyph_bitmap.buffer, bitmap_channel, glyph->bitmap_left, glyph->bitmap_top, m_height, [texture_border, &ttf_glyph, sampleInv, _doc]( uint32_t _index, int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, const void * _buffer, uint32_t _pitch, uint32_t _bytespp )
            {
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
            TTFFontTextureGlyphProvider provider( glyph_bitmap.width, glyph_bitmap.rows, glyph_bitmap.buffer, glyph_bitmap.pitch, bitmap_channel );

            mt::uv4f uv;
            RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                ->makeTextureGlyph( glyph_bitmap.width, glyph_bitmap.rows, texture_border, bitmap_channel, &provider, &uv, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( texture, "invalid make texture glyph" );

            TTFGlyphQuad & quad = ttf_glyph.quads[0];

            quad.offset.x = glyph_dx * sampleInv;
            quad.offset.y = -glyph_dy * sampleInv;
            quad.size.x = glyph_w * sampleInv;
            quad.size.y = glyph_h * sampleInv;
            quad.uv = uv;
            quad.texture = texture;
        }

        glyphs.emplace_back( ttf_glyph );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_validateGlyphes( const U32String & _codes ) const
    {
        const TTFFontGlyphPtr & ttfGlyph = this->getTTFFontGlyph();

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
        const TTFDataInterfacePtr dataTTF = m_glyph->getTTFData();

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
    bool TTFFont::hasGlyph( GlyphCode _code ) const
    {
        uint32_t code_hash = _code % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;
        const VectorTTFGlyphs & glyphs = m_glyphsHash[code_hash];

        VectorTTFGlyphs::const_iterator it_found = Algorithm::find_if( glyphs.begin(), glyphs.end(), [_code]( const TTFGlyph & _glyph )
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

        VectorTTFGlyphs::const_iterator it_found = Algorithm::find_if( glyphs.begin(), glyphs.end(), [_code]( const TTFGlyph & _glyph )
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

        const TTFDataInterfacePtr dataTTF = m_glyph->getTTFData();

        FT_Face face = this->getFTFace( dataTTF );

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

        float kerning = (float)ttf_kerning.x * MENGINE_FT_INV64;

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
