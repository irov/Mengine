
#	include "GameSettings.h"
#	include "LogEngine.h"
#	include <algorithm>
#	include "Utils.h"
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	GameSettings::GameSettings()
	{
		m_settings = ms_defaultSettings;
	}
	//////////////////////////////////////////////////////////////////////////
	GameSettings::~GameSettings()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool GameSettings::set( const String& _setting, const String& _value )
	{
		TSetttingsMap::iterator it = m_settings.find( _setting );
		if( it != m_settings.end() )
		{
			/*std::vector<String>::iterator vit = 
				std::find( 
				it->second.possibleValues.begin(), 
				it->second.possibleValues.end(), 
				_value );
			if( vit != it->second.possibleValues.end() )*/
			{
				it->second.value = _value;
				m_toApply.push_back( it->first );
				return true;
			}
		}
		MENGE_LOG( "Warning: Game Setting %s does not exist or value %s is impossible"
			, _setting.c_str()
			, _value.c_str() );
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& GameSettings::get( const String& _setting )
	{
		TSetttingsMap::iterator it = m_settings.find( _setting );
		if( it != m_settings.end() )
		{
			return it->second.value;
		}

		MENGE_LOG( "Warning: Game Setting %s does not exist"
				, _setting.c_str() );

		return emptyString();
	}
	//////////////////////////////////////////////////////////////////////////
	void GameSettings::apply()
	{
		m_toApply.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void GameSettings::setDefaults()
	{
		for( TSetttingsMap::iterator it = ms_defaultSettings.begin(), it_end = ms_defaultSettings.end()
			; it != it_end
			; it++ )
		{
			set( it->first, it->second.value );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool GameSettings::changed()
	{
		return !m_toApply.empty();
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge