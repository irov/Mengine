#	pragma once

#	include "Config/Typedef.h"
#	include <map>

namespace Menge
{
	typedef struct _tSettingValue
	{
		String value;
		std::vector<String> possibleValues;
	} TSettingValue;

	class GameSettings
	{
	public:
		GameSettings();
		~GameSettings();

		bool set( const String& _setting, const String& _value );
		const String& get( const String& _setting );

		void apply();

		static GameSettings ms_defaultSettings;
	protected:
		typedef std::map< String, TSettingValue > TSetttingsMap;
		TSetttingsMap m_settings;
		std::vector<String> m_toApply;
	};
}	// namespace Menge