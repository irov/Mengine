#	pragma once

#	include "Config/Typedef.h"

#	include <map>

namespace Menge
{
	class FileInputInterface;

	class ConfigFile
	{
	public:
		typedef std::pair<String, String> TSettingEntry;
		typedef std::vector<TSettingEntry> TSettings;

		ConfigFile();
		virtual ~ConfigFile();
		/// load from a filename (not using resource group locations)
		bool load( const String& _filename, const String& _separators = "\t:=" );
		/// load from a data stream
		bool load( FileInputInterface* _file, const String& _separators = "\t:=" );

		// Gets the first setting from the file with the named key. 
		// @param key The name of the setting
		// @param section The name of the section it must be in (if any)
		String getSetting( const String& _key, const String& _section = "" ) const;
		bool getSettingBool( const String& _key, const String& _section = "" ) const;
		int getSettingInt( const String& _key, const String& _section = "" ) const;
		unsigned int getSettingUInt( const String& _key, const String& _section = "" ) const;
		// Gets all settings from the file with the named key.
		TStringVector getMultiSetting( const String& _key, const String& _section = "" ) const;
		TSettings getSettings( const String& _section = "" ) const;

	protected:
		typedef std::multimap<String, String> TSettingsMultiMap;
		typedef std::map<String, TSettingsMultiMap*> TSettingsBySection;

		TSettingsBySection m_settings;

		// Clear the settings
		void clear_();
	};

}	// namespace Menge
