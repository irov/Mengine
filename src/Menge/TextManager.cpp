
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
	const String& TextManager::getText( const String& _key ) const
	{
		TStringMap::const_iterator it_find = m_textMap.find( _key );
		if( it_find == m_textMap.end() )
		{
			MENGE_LOG_ERROR( "Error: TextManager can't find string associated with key - \"%s\""
				, _key.c_str() );
			return Utils::emptyString();
		}
		return it_find->second;
	}
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
				addText( key, value );
			}
			XML_CASE_NODE( "Texts" )
			{
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
				addText( key, value );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::addText( const String& _key, const String& _value )
	{
		m_textMap.insert( std::make_pair( _key, _value ) );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge