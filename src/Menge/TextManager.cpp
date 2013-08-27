#	include "TextManager.h"

#	include "Core/String.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include <stdex/xml_sax_parser.h>

#   include <stdio.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TextService, Menge::TextServiceInterface, Menge::TextManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextManager::TextManager()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextManager::~TextManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void TextManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * TextManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::initialize( size_t _size )
	{
		m_texts.reserve(_size);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class TextManagerSaxCallback
		{
		public:
			TextManagerSaxCallback( ServiceProviderInterface * _serviceProvider, TextManager * _textManager, const ConstString & _pakName, const FilePath & _path )
				: m_serviceProvider(_serviceProvider)
				, m_textManager(_textManager)
				, m_pakName(_pakName)
				, m_path(_path)
			{
			}

		protected:
			void operator = ( const TextManagerSaxCallback & )
			{
			}

		public:
			void callback_begin_node( const char * _node )
			{
				(void)_node;
			}

			void callback_node_attributes( const char * _node, size_t _count, const char ** _keys, const char ** _values )
			{
				if( strcmp( _node, "Text" ) != 0 )
				{
					return;
				}

				ConstString text_key;

				TextEntry textEntry;

				textEntry.charOffset = 0.f;
				textEntry.lineOffset = 0.f;
				
				for( size_t i = 0; i != _count; ++i )
				{
					const char * key = _keys[i];
					const char * value = _values[i];

					if( strcmp(key, "Key") == 0 )
					{
						size_t value_size = strlen( value );
						text_key = Helper::stringizeStringExternal( m_serviceProvider, value, value_size );
					}
					else if( strcmp(key, "Value" ) == 0 )
					{
						size_t value_size = strlen( value );
						textEntry.text = Helper::stringizeStringExternal( m_serviceProvider, value, value_size );
					}
					else if( strcmp(key, "Font" ) == 0 )
					{
						size_t value_size = strlen( value );
						ConstString font = Helper::stringizeStringExternal( m_serviceProvider, value, value_size );

						textEntry.font = font;
					}
					else if( strcmp(key, "CharOffset" ) == 0 )
					{
						float charOffset = 0.f;
						if( sscanf( value, "%f", &charOffset ) != 1 )
						{
							LOGGER_WARNING(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s charOffset %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, value
								);
						}

						textEntry.charOffset = charOffset;
					}
					else if( strcmp(key, "LineOffset" ) == 0 )
					{
						float lineOffset = 0.f;
						if( sscanf( value, "%f", &lineOffset ) != 1 )
						{
							LOGGER_WARNING(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s lineOffset %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, value
								);
						}

						textEntry.lineOffset = lineOffset;
					}					
				}

				m_textManager->addTextEntry( text_key, textEntry );
			}

			void callback_end_node( const char * _node )
			{
				(void)_node;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			TextManager * m_textManager;

			const ConstString & m_pakName;
			const FilePath & m_path;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::loadTextEntry( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path )
	{
		TextLocalePak * pak = new TextLocalePak();

		if( pak->initialize( m_serviceProvider, _locale, _pakName, _path ) == false )
		{
			delete pak;

			return false;
		}
		
		m_paks.push_back( pak );

		char * xml_buff = pak->getXmlBuffer();

		TextManagerSaxCallback tmsc(m_serviceProvider, this, _pakName, _path);
		if( stdex::xml_sax_parse( xml_buff, tmsc ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::addTextEntry( const ConstString& _key, const TextEntry & _entry )
	{
		TextEntry * textEntry = nullptr;
		if( m_texts.has( _key, &textEntry ) == true )
		{
			LOGGER_INFO(m_serviceProvider)( "TextManager::addTextEntry: duplicate key found %s"
				, _key.c_str()
				);

			*textEntry = _entry;

            return;
		}

        m_texts.insert( _key, _entry );		
	}
	//////////////////////////////////////////////////////////////////////////
	const TextEntry & TextManager::getTextEntry( const ConstString& _key ) const
	{
		const TextEntry * textEntry = nullptr;
		if( m_texts.has( _key, &textEntry ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "TextManager::getTextEntry: TextManager can't find string associated with key - '%s'"
				, _key.c_str() 
				);

			static TextEntry emptyEntry;
			emptyEntry.charOffset = 0.f;
			emptyEntry.lineOffset = 0.f;

			return emptyEntry;
		}

        const TextEntry & entry = *textEntry;
        
        return entry;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::existText( const ConstString& _key, const TextEntry ** _entry ) const
	{
		bool result = m_texts.has( _key, _entry );

		return result;		
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::setDefaultResourceFontName( const ConstString & _fontName )
	{
		m_defaultResourceFontName = _fontName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextManager::getDefaultResourceFontName() const
	{
		return m_defaultResourceFontName;
	}
}
