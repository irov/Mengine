#	include "TextManager.h"

#	include "XmlEngine.h"

#	include "Logger/Logger.h"

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

		if( XmlEngine::get()
			->parseXmlFileM( _fileSystemName, _filename, this, &TextManager::loaderResourceFile_ ) == false )
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
	void TextManager::loaderResourceFile_( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Text" )
			{
				TextEntry textEntry;
				textEntry.lineOffset = 0.0f;
				textEntry.charOffset = 0.0f;
				ConstString key;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Key", key );
					XML_CASE_ATTRIBUTE( "Value", textEntry.text );
					XML_CASE_ATTRIBUTE( "CharOffset", textEntry.charOffset );
					XML_CASE_ATTRIBUTE( "LineOffset", textEntry.lineOffset );
					XML_CASE_ATTRIBUTE( "Font", textEntry.font );
				}
				addTextEntry( key, textEntry );
			}
			XML_CASE_NODE( "Texts" )
			{
				m_currentFont.clear();
				m_currentCharOffset = -100.0f;
				m_currentLineOffset = -100.0f;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Font", m_currentFont );
					XML_CASE_ATTRIBUTE( "CharOffset", m_currentCharOffset );
					XML_CASE_ATTRIBUTE( "LineOffset", m_currentLineOffset );
				}

				XML_PARSE_ELEMENT( this, &TextManager::loaderTexts_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::loaderTexts_( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Text" )
			{
				TextEntry textEntry;
				textEntry.lineOffset = 0.0f;
				textEntry.charOffset = 0.0f;
				ConstString key;
				String font;
				float charOffset = 0.0f;
				float lineOffset = 0.0f;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Key", key );
					XML_CASE_ATTRIBUTE( "Value", textEntry.text );
					XML_CASE_ATTRIBUTE( "CharOffset", textEntry.charOffset );
					XML_CASE_ATTRIBUTE( "LineOffset", textEntry.lineOffset );
					XML_CASE_ATTRIBUTE( "Font", textEntry.font );
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
				addTextEntry( key, textEntry );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::addTextEntry( const ConstString& _key, const TextManager::TextEntry& _entry )
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
	const TextManager::TextEntry & TextManager::getTextEntry( const ConstString& _key ) const
	{
		TStringMap::const_iterator it_find = m_textMap.find( _key );
		
		if( it_find == m_textMap.end() )
		{
			MENGE_LOG_ERROR( "Error: TextManager can't find string associated with key - '%s'"
				, _key.c_str() 
				);

			static TextEntry emptyEntry;
			emptyEntry.charOffset = 0.0f;

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
