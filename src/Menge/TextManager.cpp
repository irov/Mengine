
#	include "TextManager.h"
//#	include "ConfigFile.h"
#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "Utils.h"

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
	bool TextManager::loadResourceFile( const String& _filename )
	{
		/*m_textMap.clear();
		ConfigFile file;
		if( file.load( _filename ) == false )
		{
		MENGE_LOG_ERROR( "TextManager failed to load resource file \"%s\"", _filename.c_str() );
		return false;
		}
		ConfigFile::TSettings settings = file.getSettings();
		for( ConfigFile::TSettings::iterator it = settings.begin(), it_end = settings.end();
		it != it_end;
		it++ )
		{
		m_textMap.insert( (*it) );
		}*/

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _filename, this, &TextManager::loaderResourceFile_ ) == false )
		{
			MENGE_LOG_ERROR( "Problems parsing Text pack \"%s\""
				, _filename.c_str() );
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
	//		MENGE_LOG_ERROR( "Error: TextManager can't find string associated with key - \"%s\""
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
				String key, value;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Key", key );
					XML_CASE_ATTRIBUTE( "Value", value );
				}
				TextEntry textEntry;
				textEntry.charOffset = 0.0f;
				textEntry.text = value;
				addTextEntry( key, textEntry );
			}
			XML_CASE_NODE( "Texts" )
			{
				m_currentFont = "";
				m_currentCharOffset = 0.0f;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Font", m_currentFont );
					XML_CASE_ATTRIBUTE( "CharOffset", m_currentCharOffset );
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
				String key, value;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Key", key );
					XML_CASE_ATTRIBUTE( "Value", value );
				}
				TextEntry textEntry;
				textEntry.charOffset = m_currentCharOffset;
				textEntry.font = m_currentFont;
				textEntry.text = value;
				addTextEntry( key, textEntry );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::addTextEntry( const String& _key, const TextManager::TextEntry& _entry )
	{
		m_textMap.insert( std::make_pair( _key, _entry ) );
	}
	//////////////////////////////////////////////////////////////////////////
	TextManager::TextEntry TextManager::getTextEntry( const String& _key ) const
	{
		TStringMap::const_iterator it_find = m_textMap.find( _key );
		if( it_find == m_textMap.end() )
		{
			MENGE_LOG_ERROR( "Error: TextManager can't find string associated with key - \"%s\""
				, _key.c_str() );
			TextEntry emptyEntry;
			emptyEntry.charOffset = 0.0f;
			return emptyEntry;
		}
		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
