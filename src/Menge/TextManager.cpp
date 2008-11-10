
#	include "TextManager.h"
#	include "ConfigFile.h"
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
		m_textMap.clear();
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
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& TextManager::getText( const String& _key )
	{
		TStringMap::iterator it_find = m_textMap.find( _key );
		if( it_find == m_textMap.end() )
		{
			MENGE_LOG_ERROR( "Error: TextManager can't find string associated with key - \"%s\""
				, _key.c_str() );
			return Utils::emptyString();
		}
		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge