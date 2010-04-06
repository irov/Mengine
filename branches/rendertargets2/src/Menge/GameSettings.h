#	pragma once

#	include "Config/Typedef.h"
#	include <map>

namespace Menge
{
	/*typedef struct _tSettingValue
	{
		String value;
		std::vector<String> possibleValues;
	} TSettingValue;*/

	class GameSettings
	{
		typedef std::map< String, String > TSetttingsMap;
	public:
		GameSettings();
		~GameSettings();

		bool set( const String& _setting, const String& _value );
		const String& get( const String& _setting );

		void apply();
		bool changed();
		void setDefaults();

		static TSetttingsMap ms_defaultSettings;
	protected:
		TSetttingsMap m_settings;
		std::vector<String> m_toApply;
	};
}	// namespace Menge