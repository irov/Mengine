#	pragma once

#	include "Core/String.h"

#	include <map>

namespace Menge
{
	class FileInputInterface;

	class ConfigFile
	{
	public:
		typedef std::pair<String, String> TSettingEntry;
		typedef std::vector<TSettingEntry> TSettings;

	public:
		ConfigFile();
		virtual ~ConfigFile();

	public:
		/// load from a filename
		bool load( const ConstString& _fileSystemName, const String& _filename, const String& _separators = "\t:=" );
		/// load from a data stream
		bool load( FileInputInterface* _file, const String& _separators = "\t:=" );

	public:
		// Gets the first setting from the file with the named key. 
		// @param key The name of the setting
		// @param section The name of the section it must be in (if any)
		bool getSetting( const String& _key, const String& _section, String & _value ) const;
		//bool getSettingBool( const String& _key, const String& _section = "" ) const;
		//int getSettingInt( const String& _key, const String& _section = "" ) const;
		bool getSettingUInt( const String& _key, const String& _section, unsigned int & _value ) const;
		// Gets all settings from the file with the named key.
		//TVectorString getMultiSetting( const String& _key, const String& _section = "" ) const;
		//TSettings getSettings( const String& _section = "" ) const;

	protected:
		typedef std::multimap<String, String> TSettingsMultiMap;
		typedef std::map<String, TSettingsMultiMap*> TSettingsBySection;

		TSettingsBySection m_settings;

		// Clear the settings
		void clear_();
	};

}	// namespace Menge
