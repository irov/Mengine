#include "BitmapGlyph.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/FileGroupHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#include "xmlsax/xmlsax.hpp"

#include "utf8.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BitmapGlyph::BitmapGlyph()
        : m_size( 0.f )
        , m_height( 0 )
        , m_ascender( 0.f )
        , m_descender( 0.f )
        , m_textureInvWidth( 1.f )
        , m_textureInvHeight( 1.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BitmapGlyph::~BitmapGlyph()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapGlyph::setSize( float _size )
    {
        m_size = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapGlyph::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapGlyph::setHeight( uint32_t _height )
    {
        m_height = _height;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t BitmapGlyph::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapGlyph::setAscender( float _ascender )
    {
        m_ascender = _ascender;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapGlyph::getAscender() const
    {
        return m_ascender;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapGlyph::setDescender( float _descender )
    {
        m_descender = _descender;
    }
    //////////////////////////////////////////////////////////////////////////
    float BitmapGlyph::getDescender() const
    {
        return m_descender;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapGlyph::setTextureWidth( uint32_t _textureWidth )
    {
        m_textureInvWidth = 1.f / (float)_textureWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapGlyph::setTextureHeight( uint32_t _textureHeight )
    {
        m_textureInvHeight = 1.f / (float)_textureHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class BitmapGlyphSaxCallback
        {
        public:
            BitmapGlyphSaxCallback( BitmapGlyph * _glyph, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
                : m_glyph( _glyph )
                , m_fileGroup( _fileGroup )
                , m_filePath( _filePath )
                , m_glyphCode( 0 )
                , m_valid( false )
            {
            }

            ~BitmapGlyphSaxCallback()
            {
            }

        protected:
            BitmapGlyphSaxCallback( const BitmapGlyphSaxCallback & ) = delete;
            void operator = ( const BitmapGlyphSaxCallback & ) = delete;

        public:
            void parse( const char * _node, uint32_t _count, const char ** const _keys, const char ** const _values )
            {
                if( MENGINE_STRCMP( _node, "font" ) == 0 )
                {
                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const char * key = _keys[i];
                        const char * value = _values[i];

                        if( MENGINE_STRCMP( key, "type" ) == 0 )
                        {
                            if( MENGINE_STRCMP( value, "GHL" ) == 0 )
                            {
                                m_valid = true;
                            }
                        }
                    }
                }
                else if( MENGINE_STRCMP( _node, "description" ) == 0 )
                {
                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const char * key = _keys[i];
                        const char * value = _values[i];

                        if( MENGINE_STRCMP( key, "size" ) == 0 )
                        {
                            float size = 0.f;
                            if( Helper::stringalized( value, &size ) == false )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid read size '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }

                            m_glyph->setSize( size );
                        }
                    }
                }
                else if( MENGINE_STRCMP( _node, "metrics" ) == 0 )
                {
                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const char * key = _keys[i];
                        const char * value = _values[i];

                        if( MENGINE_STRCMP( key, "ascender" ) == 0 )
                        {
                            float ascender = 0.f;
                            if( Helper::stringalized( value, &ascender ) == false )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid read ascender '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }

                            m_glyph->setAscender( ascender );
                        }
                        else if( MENGINE_STRCMP( key, "height" ) == 0 )
                        {
                            uint32_t height = 0;
                            if( Helper::stringalized( value, &height ) == false )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid read height '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }

                            m_glyph->setHeight( height );
                        }
                        else if( MENGINE_STRCMP( key, "descender" ) == 0 )
                        {
                            float descender = 0.f;
                            if( Helper::stringalized( value, &descender ) == false )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid read descender '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }

                            m_glyph->setDescender( descender );
                        }
                    }
                }
                else if( MENGINE_STRCMP( _node, "texture" ) == 0 )
                {
                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const char * key = _keys[i];
                        const char * value = _values[i];

                        if( MENGINE_STRCMP( key, "width" ) == 0 )
                        {
                            uint32_t width = 0;
                            if( Helper::stringalized( value, &width ) == false )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid read width '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }

                            uint32_t width_pow2 = Helper::getTexturePow2( width );

                            m_glyph->setTextureWidth( width_pow2 );
                        }
                        else if( MENGINE_STRCMP( key, "height" ) == 0 )
                        {
                            uint32_t height = 0;
                            if( Helper::stringalized( value, &height ) == false )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid read height '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }

                            uint32_t height_pow2 = Helper::getTexturePow2( height );

                            m_glyph->setTextureHeight( height_pow2 );
                        }
                    }
                }
                else if( MENGINE_STRCMP( _node, "char" ) == 0 )
                {
                    float advance = 0.f;
                    mt::vec2f offset( 0.f, 0.f );
                    mt::vec4f rect( 0.f, 0.f, 0.f, 0.f );
                    const char * id = "";

                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const char * key = _keys[i];
                        const char * value = _values[i];

                        if( MENGINE_STRCMP( key, "advance" ) == 0 )
                        {
                            if( Helper::stringalized( value, &advance ) == false )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid read width '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }
                        }
                        else if( MENGINE_STRCMP( key, "offset" ) == 0 )
                        {
                            if( Helper::stringalized( value, &offset ) == false )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid read offset '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }
                        }
                        else if( MENGINE_STRCMP( key, "rect" ) == 0 )
                        {
                            if( Helper::stringalized( value, &rect ) == false )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid read rect '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }
                        }
                        else if( MENGINE_STRCMP( key, "id" ) == 0 )
                        {
                            id = value;
                        }
                    }

                    mt::vec4f uv( rect.x, rect.y, rect.x + rect.z, rect.y + rect.w );
                    mt::vec2f size( rect.z, rect.w );

                    uint32_t cp = 0;
                    size_t code_length = MENGINE_STRLEN( id );
                    utf8::internal::utf_error err_code = utf8::internal::validate_next( id, id + code_length, cp );

                    if( cp == 0 || err_code != utf8::internal::UTF8_OK )
                    {
                        LOGGER_ERROR( "glyph '%s' invalid utf8 id '%s'"
                            , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                            , id
                        );
                    }

                    GlyphCode glyphCode = cp;

                    offset.y = -offset.y;

                    m_glyph->addGlyphChar( glyphCode, uv, offset, advance, size );

                    m_glyphCode = glyphCode;
                }
                else if( MENGINE_STRCMP( _node, "kerning" ) == 0 )
                {
                    float advance = 0.f;
                    GlyphCode nextCode = 0;

                    for( uint32_t i = 0; i != _count; ++i )
                    {
                        const char * key = _keys[i];
                        const char * value = _values[i];

                        if( MENGINE_STRCMP( key, "advance" ) == 0 )
                        {
                            if( Helper::stringalized( value, &advance ) == false )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid read advance '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }
                        }
                        else if( MENGINE_STRCMP( key, "id" ) == 0 )
                        {
                            const char * id = value;

                            uint32_t cp = 0;
                            size_t id_length = MENGINE_STRLEN( id );
                            utf8::internal::utf_error err_code = utf8::internal::validate_next( id, id + id_length, cp );

                            if( cp == 0 || err_code != utf8::internal::UTF8_OK )
                            {
                                LOGGER_ERROR( "glyph '%s' invalid utf8 code '%s'"
                                    , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                                    , value
                                );
                            }

                            nextCode = cp;
                        }
                    }

                    if( m_glyphCode == 0 )
                    {
                        LOGGER_ERROR( "glyph '%s' invalid kerning m_glyphChar is 0"
                            , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
                        );

                        return;
                    }

                    m_glyph->addKerning( m_glyphCode, nextCode, advance );
                }
            }

        public:
            bool isValid() const
            {
                return m_valid;
            }

        protected:
            BitmapGlyph * m_glyph;

            const FileGroupInterfacePtr & m_fileGroup;
            const FilePath & m_filePath;

            GlyphCode m_glyphCode;
            bool m_valid;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapGlyph::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open file '%s'"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
        );

        size_t xml_buffer_size = stream->size();

        MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( xml_buffer_size + 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer );

        Char * memory = buffer->getBuffer();

        stream->read( memory, xml_buffer_size );
        memory[xml_buffer_size] = '\0';

        BitmapGlyphSaxCallback tmsc( this, _fileGroup, _filePath );

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
            LOGGER_ERROR( "invalid parse file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            return false;
        }

        if( tmsc.isValid() == false )
        {
            LOGGER_ERROR( "invalid glyph format '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapGlyph::existGlyphCode( GlyphCode _code ) const
    {
        uint32_t hash_code = _code % 257;
        const VectorGlyphChar & glyps = m_chars[hash_code];

        for( const BitmapGlyphChar & glyph : glyps )
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
    const BitmapGlyphChar * BitmapGlyph::getGlyphChar( GlyphCode _code ) const
    {
        uint32_t hash_code = _code % 257;
        const VectorGlyphChar & glyps = m_chars[hash_code];

        for( const BitmapGlyphChar & glyph : glyps )
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
    bool BitmapGlyph::addGlyphChar( GlyphCode _code, const mt::vec4f & _uv, const mt::vec2f & _offset, float _advance, const mt::vec2f & _size )
    {
        if( this->existGlyphCode( _code ) == true )
        {
            LOGGER_ERROR( "code '%u' alredy exist!"
                , _code
            );

            return false;
        }

        BitmapGlyphChar glyphChar;
        glyphChar.code = _code;
        glyphChar.uv = _uv * mt::vec2f( m_textureInvWidth, m_textureInvHeight );
        glyphChar.offset = _offset;
        glyphChar.advance = _advance;
        glyphChar.size = _size;

        uint32_t hash_code = _code % 257;
        VectorGlyphChar & glyps = m_chars[hash_code];

        glyps.emplace_back( glyphChar );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapGlyph::addKerning( GlyphCode _char, GlyphCode _next, float _kerning )
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
    float BitmapGlyph::getKerning( GlyphCode _char, GlyphCode _next ) const
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
