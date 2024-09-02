#include "BitmapFont.h"

#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Interface/FileGroupInterface.h"

#include "BitmapFontInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BitmapFont::BitmapFont()
        : m_ascender( 0.f )
        , m_descent( 0.f )
        , m_bearingYA( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BitmapFont::~BitmapFont()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFont::setEffect( const FontEffectInterfacePtr & _effect )
    {
        MENGINE_UNUSED( _effect );
    }
    //////////////////////////////////////////////////////////////////////////
    const FontEffectInterfacePtr & BitmapFont::getEffect() const
    {
        //Empty

        return FontEffectInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::initialize()
    {
        if( m_glyph == nullptr )
        {
            return false;
        }

        const BitmapFontGlyphDescriptionPtr & symbols = m_glyph->getBitmapFontGlyphDescription();

        float ascender = symbols->getAscender();
        m_ascender = ascender;

        float descender = symbols->getDescender();
        m_descent = descender;

        uint32_t height = symbols->getHeight();
        this->setHeight( height );

        const BitmapFontGlyphFace * glyphFace = symbols->getGlyphFace( MENGINE_BITMAP_METRICS_SYMBOL );

        m_bearingYA = glyphFace->size.y;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFont::finalize()
    {
        m_glyph = nullptr;

        m_textureFont = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::_compile()
    {
        const ContentInterfacePtr & content = m_glyph->getBitmapImageContent();

        m_textureFont = RENDERTEXTURE_SERVICE()
            ->loadTexture( content, DF_IMAGE_NONE, ~0U, ~0U, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_textureFont, "font '%s' invalid loading font image '%s'"
            , m_name.c_str()
            , Helper::getContentFullPath( content )
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFont::_release()
    {
        m_textureFont = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t BitmapFont::getLayoutCount() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFont::setBitmapFontGlyph( const BitmapFontGlyphPtr & _glyph )
    {
        m_glyph = _glyph;
    }
    //////////////////////////////////////////////////////////////////////////
    const BitmapFontGlyphPtr & BitmapFont::getBitmapFontGlyph() const
    {
        return m_glyph;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::getGlyph( uint32_t _layout, GlyphCode _code, GlyphCode _next, Glyph * const _glyph ) const
    {
        MENGINE_UNUSED( _layout );

        const BitmapFontGlyphDescriptionPtr & symbols = m_glyph->getBitmapFontGlyphDescription();

        const BitmapFontGlyphFace * ch = symbols->getGlyphFace( _code );

        if( ch == nullptr )
        {
            return false;
        }

        mt::uv4_from_mask( &_glyph->uv, ch->uv );
        _glyph->offset = ch->offset;
        _glyph->size = ch->size;
        _glyph->advance = ch->advance;

        _glyph->texture = m_textureFont;

        float kerning = symbols->getKerning( _code, _next );
        _glyph->advance += kerning;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::hasGlyph( GlyphCode _code ) const
    {
        const BitmapFontGlyphDescriptionPtr & symbols = m_glyph->getBitmapFontGlyphDescription();

        const BitmapFontGlyphFace * ch = symbols->getGlyphFace( _code );

        if( ch == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFont::getFontAscender() const
    {
        return m_ascender;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFont::getFontDescender() const
    {
        return m_descent;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFont::getFontHeight() const
    {
        return (float)m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFont::getFontBearingYA() const
    {
        return m_bearingYA;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFont::getFontSpacing() const
    {
        return 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::getFontPremultiply() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::_validateGlyphes( const U32String & _codes ) const
    {
        bool successful = true;

        for( GlyphCode bitmap_code : _codes )
        {
            if( this->hasGlyph( bitmap_code ) == false )
            {
                LOGGER_ERROR( "bitmap font '%s' not found glyph code [%u]"
                    , this->getName().c_str()
                    , bitmap_code
                );

                successful = false;

                continue;
            }
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::_prepareGlyph( uint32_t _code, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _code );
        MENGINE_UNUSED( _doc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}