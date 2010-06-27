#	include "ConfigFile.h"

#	include "FileEngine.h"
#	include "Utils/Core/File.h"
#	include "Utils/Core/String.h"

#	include <cstdio>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConfigFile::ConfigFile()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConfigFile::~ConfigFile()
	{
		clear_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::load( const ConstString& _fileSystemName, const String& _filename, const String& _separators )
	{
		FileInputInterface* file = FileEngine::get()
			->openInputFile( _fileSystemName, _filename );

		if( file == NULL )
		{
			return false;
		}
		bool res = load( file );

		file->close();

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::load( FileInputInterface* _file, const String& _separators )
	{
		if( _file == 0 )
		{
			return false;
		}
		// Clear current settings map
		clear_();

		String currentSection;
		TSettingsMultiMap* currentSettings = new TSettingsMultiMap();
		m_settings[currentSection] = currentSettings;

		// Process the file line for line
		String line, optName, optVal;

		// check for UTF8 BOM
		unsigned char bom[3];
		_file->read( &bom, 3 );
		if( ( bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF ) == false )
		{
			_file->seek( 0 );
		}
		while ( Utils::eof( _file ) == false )
		{
			line = Utils::getLine( _file );
			// Ignore comments & blanks
			if( line.length() > 0 && line.at(0) != '#' && line.at(0) != '@' )
			{
				if( line.at(0) == '[' && line.at(line.length()-1) == ']' )
				{
					// Section
					currentSection = line.substr(1, line.length() - 2);
					TSettingsBySection::const_iterator seci = m_settings.find( currentSection );
					if( seci == m_settings.end() )
					{
						currentSettings = new TSettingsMultiMap();
						m_settings[currentSection] = currentSettings;
					}
					else
					{
						currentSettings = seci->second;
					} 
				}
				else
				{
					// Find the first seperator character and split the string there
					String::size_type separator_pos = line.find_first_of( _separators, 0 );
					if (separator_pos != String::npos)
					{
						optName = line.substr(0, separator_pos);
						// Find the first non-seperator character following the name
						String::size_type nonseparator_pos = line.find_first_not_of( _separators, separator_pos );
						// ... and extract the value
						// Make sure we don't crash on an empty setting (it might be a valid value)
						optVal = (nonseparator_pos == String::npos) ? "" : line.substr(nonseparator_pos);

						// trimWhitespace
						Utils::trim(optVal);
						Utils::trim(optName);

						currentSettings->insert(std::multimap<String, String>::value_type(optName, optVal));
					}
				}
			}
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ConfigFile::getSetting( const String& _key, const String& _section /*= "" */ ) const
	{
		TSettingsBySection::const_iterator seci = m_settings.find( _section );
		if (seci == m_settings.end())
		{
			return Utils::emptyString();
		}
		else
		{
			TSettingsMultiMap::const_iterator i = seci->second->find( _key );
			if (i == seci->second->end())
			{
				return Utils::emptyString();
			}
			else
			{
				return i->second;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TVectorString ConfigFile::getMultiSetting( const String& _key, const String& _section /*= "" */ ) const
	{
		TVectorString ret;

		TSettingsBySection::const_iterator seci = m_settings.find( _section );
		if (seci != m_settings.end())
		{
			TSettingsMultiMap::const_iterator i;

			i = seci->second->find( _key );
			// Iterate over matches
			while (i != seci->second->end() && i->first == _key )
			{
				ret.push_back(i->second);
				++i;
			}
		}
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	void ConfigFile::clear_()
	{
		TSettingsBySection::iterator seci, secend;
		secend = m_settings.end();
		for (seci = m_settings.begin(); seci != secend; ++seci)
		{
			delete seci->second;
		}
		m_settings.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConfigFile::getSettingBool( const String& _key, const String& _section /*= "" */ ) const
	{
		unsigned int val = 0;
		String setting = getSetting( _key, _section );
		std::sscanf( setting.c_str(), "%u", &val );
		return ( val != 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	int ConfigFile::getSettingInt( const String& _key, const String& _section /*= "" */ ) const
	{
		int val = 0;
		String setting = getSetting( _key, _section );
		std::sscanf( setting.c_str(), "%d", &val );
		return val;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ConfigFile::getSettingUInt( const String& _key, const String& _section /*= "" */ ) const
	{
		unsigned int val = 0;
		String setting = getSetting( _key, _section );
		std::sscanf( setting.c_str(), "%u", &val );
		return val;
	}
	//////////////////////////////////////////////////////////////////////////
	ConfigFile::TSettings ConfigFile::getSettings( const String& _section /*= "" */ ) const
	{
		TSettings ret;

		TSettingsBySection::const_iterator seci = m_settings.find( _section );
		if (seci != m_settings.end())
		{
			for( TSettingsMultiMap::iterator it = seci->second->begin(), it_end = seci->second->end();
				it != it_end; it++ )
			{
				ret.push_back(*it);
			}
		}
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
