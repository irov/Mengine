#	include "TextGlyph.h"

#	include "Interface/StreamInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/CacheInterface.h"

#	include "Core/CacheMemoryBuffer.h"

#	include "Logger/Logger.h"

#	include "stdex/xml_sax_parser.h"

 #	include <utf8.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextGlyph::TextGlyph()
		: m_serviceProvider(nullptr)
		, m_size(0.f)
		, m_height(0.f)
		, m_ascender(0.f)
		, m_descender(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextGlyph::~TextGlyph()
	{
		m_chars.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextGlyph::setSize( float _size )
	{
		m_size = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextGlyph::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextGlyph::setHeight( float _height )
	{
		m_height = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextGlyph::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextGlyph::setAscender( float _ascender )
	{
		m_ascender = _ascender;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextGlyph::getAscender() const
	{
		return m_ascender;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextGlyph::setDescender( float _descender )
	{
		m_descender = _descender;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextGlyph::getDescender() const
	{
		return m_descender;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class TextGlyphSaxCallback
		{
		public:
			TextGlyphSaxCallback( ServiceProviderInterface * _serviceProvider, TextGlyph * _textGlyph, const ConstString & _pakName, const FilePath & _path )
				: m_serviceProvider(_serviceProvider)				
				, m_textGlyph(_textGlyph)
				, m_pakName(_pakName)
				, m_path(_path)
				, m_glyphChar(nullptr)
				, m_valid(false)
			{
			}

		protected:
			void operator = ( const TextGlyphSaxCallback & )
			{
			}

		public:
			void callback_begin_node( const char * _node )
			{
				(void)_node;
			}

			void callback_node_attributes( const char * _node, uint32_t _count, const char ** _keys, const char ** _values )
			{				
				if( strcmp( _node, "font" ) == 0 )
				{
					for( uint32_t i = 0; i != _count; ++i )
					{
						const char * key = _keys[i];
						const char * value = _values[i];

						if( strcmp( key, "type" ) == 0 )
						{
							if( strcmp( value, "GHL" ) == 0 )
							{
								m_valid = true;
							}
						}
					}
				}
				else if( strcmp( _node, "description" ) == 0 )
				{
					for( uint32_t i = 0; i != _count; ++i )
					{
						const char * key = _keys[i];
						const char * value = _values[i];

						if( strcmp( key, "size" ) == 0 )
						{
							float size;
							if( sscanf( value, "%f", &size ) != 1 )
							{
								LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid read size %s"
									, m_pakName.c_str()
									, m_path.c_str()
									, value
									);
							}

							m_textGlyph->setSize( size );
						}
					}
				}
				else if( strcmp( _node, "metrics" ) == 0 )
				{
					for( uint32_t i = 0; i != _count; ++i )
					{
						const char * key = _keys[i];
						const char * value = _values[i];

						if( strcmp( key, "ascender" ) == 0 )
						{
							float ascender = 0.f;
							if( sscanf( value, "%f", &ascender ) != 1 )
							{
								LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid read ascender %s"
									, m_pakName.c_str()
									, m_path.c_str()
									, value
									);
							}

							m_textGlyph->setAscender( ascender );
						}
						else if( strcmp( key, "height" ) == 0 )
						{
							float height = 0.f;
							if( sscanf( value, "%f", &height ) != 1 )
							{
								LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid read height %s"
									, m_pakName.c_str()
									, m_path.c_str()
									, value
									);
							}

							m_textGlyph->setHeight( height );
						}
						else if( strcmp( key, "descender" ) == 0 )
						{
							float descender = 0.f;
							if( sscanf( value, "%f", &descender ) != 1 )
							{
								LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid read descender %s"
									, m_pakName.c_str()
									, m_path.c_str()
									, value
									);
							}

							m_textGlyph->setDescender( descender );
						}
					}
				}
				else if( strcmp( _node, "char" ) == 0 )
				{
					float advance = 0.f;
					mt::vec2f offset(0.f, 0.f);
					mt::vec4f rect(0.f, 0.f, 0.f, 0.f);
					const char * id = "";
					
					for( uint32_t i = 0; i != _count; ++i )
					{
						const char * key = _keys[i];
						const char * value = _values[i];

						if( strcmp( key, "advance" ) == 0 )
						{							
							if( sscanf( value, "%f", &advance ) != 1 )
							{
								LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid read width %s"
									, m_pakName.c_str()
									, m_path.c_str()
									, value
									);
							}
						}
						else if( strcmp( key, "offset" ) == 0 )
						{
							if( sscanf( value, "%f %f", &offset.x, &offset.y ) != 2 )
							{
								LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid read offset %s"
									, m_pakName.c_str()
									, m_path.c_str()
									, value
									);
							}
						}
						else if( strcmp( key, "rect" ) == 0 )
						{
							if( sscanf( value, "%f %f %f %f", &rect.x, &rect.y, &rect.z, &rect.w ) != 4 )
							{
								LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid read rect %s"
									, m_pakName.c_str()
									, m_path.c_str()
									, value
									);
							}
						}
						else if( strcmp( key, "id" ) == 0 )
						{
							id = value;
						}
					}

					mt::vec4f uv(rect.x, rect.y, rect.x + rect.z, rect.y + rect.w);
					mt::vec2f size(rect.z, rect.w);

					uint32_t cp = 0;
					size_t code_length = strlen(id);
					utf8::internal::utf_error err_code = utf8::internal::validate_next( id, id + code_length, cp );

					if( cp == 0 || err_code != utf8::internal::UTF8_OK )
					{
						LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid utf8 id %s"
							, m_pakName.c_str()
							, m_path.c_str()
							, id
							);
					}

					GlyphCode glyphCode;
					glyphCode.setCode( cp );

					float ascender = m_textGlyph->getAscender();
					offset.y = ascender - offset.y;

					m_glyphChar = m_textGlyph->addGlyphChar( glyphCode, uv, offset, advance, size );
				}
				else if( strcmp( _node, "kerning" ) == 0 )
				{	
					float advance = 0.f;
					GlyphCode glyphCode;					

					for( uint32_t i = 0; i != _count; ++i )
					{
						const char * key = _keys[i];
						const char * value = _values[i];

						if( strcmp( key, "advance" ) == 0 )
						{					
							if( sscanf( value, "%f", &advance ) != 1 )
							{
								LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid read advance %s"
									, m_pakName.c_str()
									, m_path.c_str()
									, value
									);
							}
						}
						else if( strcmp( key, "id" ) == 0 )
						{
							const char * id = value;

							uint32_t cp = 0;
							size_t id_length = strlen(id);
							utf8::internal::utf_error err_code = utf8::internal::validate_next( id, id + id_length, cp );

							if( cp == 0 || err_code != utf8::internal::UTF8_OK )
							{
								LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid utf8 code %s"
									, m_pakName.c_str()
									, m_path.c_str()
									, value
									);
							}

							glyphCode.setCode( cp );
						}
					}

					if( m_glyphChar == nullptr )
					{
						LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize %s:%s invalid kerning m_glyphChar is nullptr"
							, m_pakName.c_str()
							, m_path.c_str()
							);

						return;
					}

					m_glyphChar->addKerning( glyphCode, advance );
				}
			}

			void callback_end_node( const char * _node )
			{
				(void)_node;
			}

		public:
			bool isValid() const
			{
				return m_valid;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			TextGlyph * m_textGlyph;

			const ConstString & m_pakName;
			const FilePath & m_path;

			TextGlyphChar * m_glyphChar;
			bool m_valid;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextGlyph::initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _locale, const ConstString & _pakName, const FilePath & _path )
	{
		m_serviceProvider = _serviceProvider;
		m_locale = _locale;

		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _pakName, _path, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize invalid open file %s:%s"
				, _pakName.c_str()
				, _path.c_str()
				);

			return false;
		}

		size_t xml_buffer_size = stream->size();
		
		CacheMemoryBuffer buffer(m_serviceProvider, xml_buffer_size + 1, "TextGlyph");
		char * memory = buffer.getMemoryT<char>();

		stream->read( memory, xml_buffer_size );
		memory[xml_buffer_size] = 0;

		TextGlyphSaxCallback tmsc(m_serviceProvider, this, _pakName, _path);
		if( stdex::xml_sax_parse( memory, tmsc ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize invalid parse file %s:%s"
				, _pakName.c_str()
				, _path.c_str()
				);

			return false;
		}

		if( tmsc.isValid() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("TextGlyph::initialize invalid glyph format %s:%s"
				, _pakName.c_str()
				, _path.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TextGlyphChar * TextGlyph::getGlyphChar( GlyphCode _code ) const
	{
		const TextGlyphChar * ch = m_chars.find( _code );

		return ch;
	}
	//////////////////////////////////////////////////////////////////////////
	TextGlyphChar * TextGlyph::addGlyphChar( GlyphCode _code, const mt::vec4f & _uv, const mt::vec2f & _offset, float _advance, const mt::vec2f & _size )
	{
		if( m_chars.exist( _code ) == true )
		{
			LOGGER_ERROR(m_serviceProvider)("TextGlyph::addGlyphChar code %d alredy exist!"
				, _code.getCode()
				);

			return nullptr;
		}

		TextGlyphChar * glyphChar = m_chars.create();

		glyphChar->initialize( _code, _uv, _offset, _advance, _size );

		m_chars.insert( glyphChar );

		return glyphChar;
	}
}