#	include "TextManager.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "Logger/Logger.h"

#	include <sstream>

//#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextManager::TextManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextManager::~TextManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::loadResourceFile( const ConstString& _fileSystemName, const String& _filename )
	{
		bool exist = false;

		if( LoaderEngine::get()
			->load( _fileSystemName, _filename, this, exist ) == false )
		//if( XmlEngine::get()
		//	->parseXmlFileM( _fileSystemName, _filename, this, &TextManager::loaderResourceFile_ ) == false )
		{
			MENGE_LOG_ERROR( "Problems parsing Text pack '%s'"
				, _filename.c_str() 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	//const String& TextManager::getText( const String& _key ) const
	//{
	//	TStringMap::const_iterator it_find = m_textMap.find( _key );
	//	if( it_find == m_textMap.end() )
	//	{
	//		MENGE_LOG_ERROR( "Error: TextManager can't find string associated with key - '%s'"
	//			, _key.c_str() );
	//		return Utils::emptyString();
	//	}
	//	return it_find->second;
	//}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Texts )
			{
				m_currentFont.clear();
				m_currentCharOffset = -100.0f;
				m_currentLineOffset = -100.0f;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Texts_Font, m_currentFont );
					BIN_CASE_ATTRIBUTE( Protocol::Texts_CharOffset, m_currentCharOffset );
					BIN_CASE_ATTRIBUTE( Protocol::Texts_LineOffset, m_currentLineOffset );
				}

				BIN_PARSE_METHOD( this, &TextManager::loaderTexts_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::_loaded()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::loaderTexts_( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Text )
			{
				TextEntry textEntry;
				textEntry.lineOffset = 0.0f;
				textEntry.charOffset = 0.0f;
				ConstString key;
				String font;
				float charOffset = 0.0f;
				float lineOffset = 0.0f;
				
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Text_Key, key );
					BIN_CASE_ATTRIBUTE( Protocol::Text_Value, textEntry.text );
					BIN_CASE_ATTRIBUTE( Protocol::Text_CharOffset, textEntry.charOffset );
					BIN_CASE_ATTRIBUTE( Protocol::Text_LineOffset, textEntry.lineOffset );
					BIN_CASE_ATTRIBUTE( Protocol::Text_Font, textEntry.font );
				}

				if( key.empty() == true )
				{
					continue;
				}

				if( m_currentFont.empty() == false )
				{
					textEntry.font = m_currentFont;
				}
				if( m_currentCharOffset > -100.0f )
				{
					textEntry.charOffset = m_currentCharOffset;
				}
				if( m_currentLineOffset > -100.0f )
				{
					textEntry.lineOffset = m_currentLineOffset;
				}

				std::stringstream trimmer;
				trimmer << key.to_str();

				String trim_key;
				trimmer >> trim_key;

				ConstString c_trimmer_key(trim_key);

				this->addTextEntry( c_trimmer_key, textEntry );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::addTextEntry( const ConstString& _key, const TextEntry & _entry )
	{
		TStringMap::iterator it = m_textMap.find( _key );
		if( it != m_textMap.end() )
		{
			MENGE_LOG_ERROR( "Warning: TextManager duplicate key found %s"
				, _key.c_str() 
				);

			return;
		}

		m_textMap.insert( std::make_pair( _key, _entry ) );
	}
	//////////////////////////////////////////////////////////////////////////
	const TextEntry & TextManager::getTextEntry( const ConstString& _key ) const
	{
		TStringMap::const_iterator it_find = m_textMap.find( _key );
		
		if( it_find == m_textMap.end() )
		{
			MENGE_LOG_ERROR( "Error: TextManager can't find string associated with key - '%s'"
				, _key.c_str() 
				);

			static TextEntry emptyEntry;
			emptyEntry.charOffset = 0.0f;
			emptyEntry.lineOffset = 0.0f;

			return emptyEntry;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::existText( const ConstString& _key ) const
	{
		TStringMap::const_iterator it_find = m_textMap.find( _key );
		return it_find != m_textMap.end();
	}
}
