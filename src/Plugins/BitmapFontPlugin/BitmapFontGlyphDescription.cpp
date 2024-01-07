#include "BitmapFontGlyphDescription.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/Utf8Helper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#include "xmlsax/xmlsax.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BitmapFontGlyphDescription::BitmapFontGlyphDescription()
        : m_size( 0.f )
        , m_height( 0 )
        , m_ascender( 0.f )
        , m_descender( 0.f )
        , m_textureInvWidth( 1.f )
        , m_textureInvHeight( 1.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BitmapFontGlyphDescription::~BitmapFontGlyphDescription()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyphDescription::setSize( float _size )
    {
        m_size = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFontGlyphDescription::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyphDescription::setHeight( uint32_t _height )
    {
        m_height = _height;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t BitmapFontGlyphDescription::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyphDescription::setAscender( float _ascender )
    {
        m_ascender = _ascender;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFontGlyphDescription::getAscender() const
    {
        return m_ascender;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyphDescription::setDescender( float _descender )
    {
        m_descender = _descender;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFontGlyphDescription::getDescender() const
    {
        return m_descender;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyphDescription::setTextureWidth( uint32_t _textureWidth )
    {
        m_textureInvWidth = 1.f / (float)_textureWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyphDescription::setTextureHeight( uint32_t _textureHeight )
    {
        m_textureInvHeight = 1.f / (float)_textureHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class BitmapGlyphSaxCallback
        {
        public:
            BitmapGlyphSaxCallback( BitmapFontGlyphDescription * _glyph, const ContentInterfacePtr & _content )
                : m_symbols( _glyph )
                , m_content( _content )
                , m_glyphCode( 0 )
                , m_successful( false )
            {
            }

            ~BitmapGlyphSaxCallback()
            {
            }

        protected:
            BitmapGlyphSaxCallback( const BitmapGlyphSaxCallback & ) = delete;
            void operator = ( const BitmapGlyphSaxCallback & ) = delete;

        public:
            void parse( const xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** const _keys, const xmlsax_char_t ** const _values )
            {
                if( MENGINE_STRCMP( _node, "font" ) == 0 )
                {
                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const xmlsax_char_t * key = _keys[i];
                        const xmlsax_char_t * value = _values[i];

                        if( MENGINE_STRCMP( key, "type" ) == 0 )
                        {
                            if( MENGINE_STRCMP( value, "GHL" ) == 0 )
                            {
                                m_successful = true;
                            }
                        }
                    }
                }
                else if( MENGINE_STRCMP( _node, "description" ) == 0 )
                {
                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const xmlsax_char_t * key = _keys[i];
                        const xmlsax_char_t * value = _values[i];

                        if( MENGINE_STRCMP( key, "size" ) == 0 )
                        {
                            float size = 0.f;
                            if( Helper::stringalized( value, &size ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid read size '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }

                            m_symbols->setSize( size );
                        }
                    }
                }
                else if( MENGINE_STRCMP( _node, "metrics" ) == 0 )
                {
                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const xmlsax_char_t * key = _keys[i];
                        const xmlsax_char_t * value = _values[i];

                        if( MENGINE_STRCMP( key, "ascender" ) == 0 )
                        {
                            float ascender = 0.f;
                            if( Helper::stringalized( value, &ascender ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid read ascender '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }

                            m_symbols->setAscender( ascender );
                        }
                        else if( MENGINE_STRCMP( key, "height" ) == 0 )
                        {
                            uint32_t height = 0;
                            if( Helper::stringalized( value, &height ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid read height '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }

                            m_symbols->setHeight( height );
                        }
                        else if( MENGINE_STRCMP( key, "descender" ) == 0 )
                        {
                            float descender = 0.f;
                            if( Helper::stringalized( value, &descender ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid read descender '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }

                            m_symbols->setDescender( descender );
                        }
                    }
                }
                else if( MENGINE_STRCMP( _node, "texture" ) == 0 )
                {
                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const xmlsax_char_t * key = _keys[i];
                        const xmlsax_char_t * value = _values[i];

                        if( MENGINE_STRCMP( key, "width" ) == 0 )
                        {
                            uint32_t width = 0;
                            if( Helper::stringalized( value, &width ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid read width '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }

                            uint32_t width_pow2 = Helper::getTexturePow2( width );

                            m_symbols->setTextureWidth( width_pow2 );
                        }
                        else if( MENGINE_STRCMP( key, "height" ) == 0 )
                        {
                            uint32_t height = 0;
                            if( Helper::stringalized( value, &height ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid read height '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }

                            uint32_t height_pow2 = Helper::getTexturePow2( height );

                            m_symbols->setTextureHeight( height_pow2 );
                        }
                    }
                }
                else if( MENGINE_STRCMP( _node, "char" ) == 0 )
                {
                    float advance = 0.f;
                    mt::vec2f offset( 0.f, 0.f );
                    mt::vec4f rect( 0.f, 0.f, 0.f, 0.f );
                    const xmlsax_char_t * id = "";

                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const xmlsax_char_t * key = _keys[i];
                        const xmlsax_char_t * value = _values[i];

                        if( MENGINE_STRCMP( key, "advance" ) == 0 )
                        {
                            if( Helper::stringalized( value, &advance ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid read width '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }
                        }
                        else if( MENGINE_STRCMP( key, "offset" ) == 0 )
                        {
                            if( Helper::stringalized( value, &offset ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid read offset '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }
                        }
                        else if( MENGINE_STRCMP( key, "rect" ) == 0 )
                        {
                            if( Helper::stringalized( value, &rect ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid read rect '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }
                        }
                        else if( MENGINE_STRCMP( key, "id" ) == 0 )
                        {
                            id = value;
                        }
                    }

                    mt::vec4f uv( rect.x, rect.y, rect.x + rect.z, rect.y + rect.w );
                    mt::vec2f size( rect.z, rect.w );

                    uint32_t code = 0;
                    size_t code_length = MENGINE_STRLEN( id );

                    if( Helper::Utf8NextCode( &id, id + code_length, &code ) == false )
                    {
                        LOGGER_ERROR( "bitmap glyph '%s' invalid utf8 id '%s'"
                            , Helper::getContentFullPath( m_content )
                            , id
                        );

                        this->setError();
                    }

                    if( code == 0 )
                    {
                        LOGGER_ERROR( "bitmap glyph '%s' null utf8 id '%s'"
                            , Helper::getContentFullPath( m_content )
                            , id
                        );

                        this->setError();
                    }

                    GlyphCode glyphCode = code;

                    offset.y = -offset.y;

                    m_symbols->addGlyphCode( glyphCode, uv, offset, advance, size );

                    m_glyphCode = glyphCode;
                }
                else if( MENGINE_STRCMP( _node, "kerning" ) == 0 )
                {
                    float advance = 0.f;
                    GlyphCode nextCode = 0;

                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const xmlsax_char_t * key = _keys[i];
                        const xmlsax_char_t * value = _values[i];

                        if( MENGINE_STRCMP( key, "advance" ) == 0 )
                        {
                            if( Helper::stringalized( value, &advance ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid read advance '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }
                        }
                        else if( MENGINE_STRCMP( key, "id" ) == 0 )
                        {
                            const xmlsax_char_t * id = value;

                            uint32_t code;
                            size_t id_length = MENGINE_STRLEN( id );

                            if( Helper::Utf8NextCode( &id, id + id_length, &code ) == false )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' invalid utf8 code '%s'"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }

                            if( code == 0 )
                            {
                                LOGGER_ERROR( "bitmap glyph '%s' utf8 code [%s] is zero"
                                    , Helper::getContentFullPath( m_content )
                                    , value
                                );

                                this->setError();
                            }

                            nextCode = code;
                        }
                    }

                    if( m_glyphCode == 0 )
                    {
                        LOGGER_ERROR( "bitmap glyph '%s' invalid kerning m_glyphChar is 0"
                            , Helper::getContentFullPath( m_content )
                        );

                        this->setError();
                    }

                    m_symbols->addKerning( m_glyphCode, nextCode, advance );
                }
            }

        public:
            void setError()
            {
                m_successful = false;
            }

            bool isValid() const
            {
                return m_successful;
            }

        protected:
            BitmapFontGlyphDescription * m_symbols;

            const ContentInterfacePtr & m_content;

            GlyphCode m_glyphCode;
            bool m_successful;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontGlyphDescription::initialize( const ContentInterfacePtr & _content )
    {
        InputStreamInterfacePtr stream = _content->openInputStreamFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open file '%s'"
            , Helper::getContentFullPath( _content )
        );

        size_t xml_buffer_size = stream->size();

        MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( xml_buffer_size + 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer );

        Char * memory = buffer->getBuffer();

        stream->read( memory, xml_buffer_size );
        memory[xml_buffer_size] = '\0';

        BitmapGlyphSaxCallback tmsc( this, _content );

        xmlsax_callbacks_t callbacks;
        callbacks.begin_node = []( const xmlsax_char_t *, void * )
        {};
        callbacks.end_node = []( const xmlsax_char_t *, void * )
        {};

        callbacks.node_attributes = []( const xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** const _key, const xmlsax_char_t ** const _value, void * _userdata )
        {
            MENGINE_UNUSED( _userdata );

            BitmapGlyphSaxCallback * callback = static_cast<BitmapGlyphSaxCallback *>(_userdata);

            callback->parse( _node, _count, _key, _value );
        };

        if( xmlsax_parse( memory, &callbacks, &tmsc ) == false )
        {
            LOGGER_ERROR( "bitmap invalid parse file '%s'"
                , Helper::getContentFullPath( _content )
            );

            return false;
        }

        if( tmsc.isValid() == false )
        {
            LOGGER_ERROR( "bitmap invalid glyph format '%s'"
                , Helper::getContentFullPath( _content )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontGlyphDescription::existGlyphCode( GlyphCode _code ) const
    {
        uint32_t hash_code = _code % 257;
        const VectorFontGlyphFace & glyphs = m_chars[hash_code];

        for( const BitmapFontGlyphFace & glyph : glyphs )
        {
            if( glyph.code != _code )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const BitmapFontGlyphFace * BitmapFontGlyphDescription::getGlyphFace( GlyphCode _code ) const
    {
        uint32_t hash_code = _code % 257;
        const VectorFontGlyphFace & glyphs = m_chars[hash_code];

        for( const BitmapFontGlyphFace & glyph : glyphs )
        {
            if( glyph.code != _code )
            {
                continue;
            }

            return &glyph;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontGlyphDescription::addGlyphCode( GlyphCode _code, const mt::vec4f & _uv, const mt::vec2f & _offset, float _advance, const mt::vec2f & _size )
    {
        if( this->existGlyphCode( _code ) == true )
        {
            LOGGER_ERROR( "bitmap code '%u' already exist!"
                , _code
            );

            return false;
        }

        BitmapFontGlyphFace glyphFace;
        glyphFace.code = _code;
        glyphFace.uv = _uv * mt::vec2f( m_textureInvWidth, m_textureInvHeight );
        glyphFace.offset = _offset;
        glyphFace.advance = _advance;
        glyphFace.size = _size;

        uint32_t hash_code = _code % 257;
        VectorFontGlyphFace & glyphs = m_chars[hash_code];

        glyphs.emplace_back( glyphFace );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyphDescription::addKerning( GlyphCode _char, GlyphCode _next, float _kerning )
    {
        uint32_t hash_code = (_char + _next) % 257;
        VectorKerning & kerning = m_kernings[hash_code];

        KerningDesc desc;
        desc.code = _char;
        desc.next = _next;
        desc.kerning = _kerning;

        kerning.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapFontGlyphDescription::getKerning( GlyphCode _char, GlyphCode _next ) const
    {
        uint32_t hash_code = (_char + _next) % 257;
        const VectorKerning & kerning = m_kernings[hash_code];

        for( const KerningDesc & desc : kerning )
        {
            if( desc.code != _char || desc.next != _next )
            {
                continue;
            }

            return desc.kerning;
        }

        return 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
}
