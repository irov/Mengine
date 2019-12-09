#include "BitmapFont.h"

#include "Interface/CodecServiceInterface.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Interface/FileGroupInterface.h"

#include "BitmapFontInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BitmapFont::BitmapFont()
        : m_height( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BitmapFont::~BitmapFont()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::initialize( const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config )
    {
        m_fileGroup = _fileGroup;

        if( this->initializeBase_( _config ) == false )
        {
            return false;
        }

        FilePath glyphPath;
        if( _config->hasValue( m_name.c_str(), "Glyph", &glyphPath ) == false )
        {
            LOGGER_ERROR( "invalid font '%s' don't setup Glyph"
                , m_name.c_str()
            );

            return false;
        }

        BitmapGlyphPtr glyph = BITMAPGLYPH_SERVICE()
            ->getGlyph( _fileGroup, glyphPath );

        MENGINE_ASSERTION_MEMORY_PANIC( glyph, false, "invalid font '%s' don't load Glyph '%s'"
            , m_name.c_str()
            , glyphPath.c_str()
        );

        m_glyph = glyph;

        m_height = m_glyph->getHeight();

        if( _config->hasValue( m_name.c_str(), "Image", &m_pathFontImage ) == false )
        {
            LOGGER_ERROR( "invalid font '%s' dont setup Image"
                , m_name.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFont::finalize()
    {
        m_glyph = nullptr;

        m_fileGroup = nullptr;

        m_textureFont = nullptr;
        m_textureOutline = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::isValid()
    {
        if( m_fileGroup->existFile( m_pathFontImage, true ) == false )
        {
            LOGGER_ERROR( "font '%s' not found file '%s'"
                , m_name.c_str()
                , m_pathFontImage.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::_compile()
    {
        const ConstString & fontImageCodec = CODEC_SERVICE()
            ->findCodecType( m_pathFontImage );

        m_textureFont = RENDERTEXTURE_SERVICE()
            ->loadTexture( m_fileGroup, m_pathFontImage, fontImageCodec, DF_NONE, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_textureFont, false, "font '%s' invalid loading font image '%s'"
            , m_name.c_str()
            , m_pathFontImage.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFont::_release()
    {
        m_textureFont = nullptr;
        m_textureOutline = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t BitmapFont::getLayoutCount() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFont::setGlyph( const BitmapGlyphPtr & _glyph )
    {
        m_glyph = _glyph;

        m_height = m_glyph->getHeight();
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::getGlyph( uint32_t _layout, GlyphCode _code, GlyphCode _next, Glyph * _glyph ) const
    {
        MENGINE_UNUSED( _layout );

        const BitmapGlyphChar * ch = m_glyph->getGlyphChar( _code );

        if( ch == nullptr )
        {
            return false;
        }

        mt::uv4_from_mask( _glyph->uv, ch->uv );
        _glyph->offset = ch->offset;
        _glyph->size = ch->size;
        _glyph->advance = ch->advance;

        _glyph->texture = m_textureFont;

        float kerning = m_glyph->getKerning( _code, _next );
        _glyph->advance += kerning;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFont::hasGlyph( GlyphCode _code ) const
    {
        const BitmapGlyphChar * ch = m_glyph->getGlyphChar( _code );

        if( ch == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFont::getFontAscent() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFont::getFontDescent() const
    {
        return 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFont::getFontHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFont::getFontBearingYA() const
    {
        return m_height;
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

        for( U32String::const_iterator
            it = _codes.begin(),
            it_end = _codes.end();
            it != it_end;
            ++it )
        {
            GlyphCode bitmap_code = *it;

            if( this->hasGlyph( bitmap_code ) == false )
            {
                LOGGER_ERROR( "bitmap font '%s' not found glyph code '%d'"
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
    bool BitmapFont::_prepareGlyph( uint32_t _code )
    {
        MENGINE_UNUSED( _code );

        return true;
    }
}