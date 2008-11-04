#	include "Account.h"
#	include "LogEngine.h"
#	include "pybind/pybind.hpp"
#	include "Utils.h"
#	include "FileEngine.h"
#	include "XmlEngine.h"
#	include "ConfigFile.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Account::Account( const String& _name )
		: m_name( _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Account::~Account()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Account::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::addSetting( const String& _setting, const String& _defaultValue, PyObject* _applyFunc )
	{
		TSettingsMap::iterator it = m_settings.find( _setting );
		if( it == m_settings.end() )
		{
			m_settings[_setting] = std::make_pair( _defaultValue, _applyFunc );
		}
		else
		{
			MENGE_LOG_ERROR( "Warning: Setting \"%s\" already exist"
				, _setting.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::changeSetting( const String& _setting, const String& _value )
	{
		TSettingsMap::iterator it = m_settings.find( _setting );
		if( it != m_settings.end() )
		{
			pybind::call( it->second.second, "ss", _setting.c_str(), _value.c_str() );
			it->second.first = _value;
		}
		else
		{
			MENGE_LOG_ERROR( "setting \"%s\" does not exist. Can't change"
				, _setting.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Account::getSetting( const String& _setting )
	{
		TSettingsMap::iterator it = m_settings.find( _setting );
		if( it != m_settings.end() )
		{
			return it->second.first;
		}
		else
		{
			MENGE_LOG_ERROR( "setting \"%s\" does not exist. Can't get"
				, _setting.c_str() );
		}
		return Utils::emptyString();
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::load()
	{
		FileEngine* fileEngine = Holder<FileEngine>::hostage();
		String fileName = fileEngine->getAppDataPath() + "/" + m_name + "/settings.ini";
		ConfigFile config;
		if( config.load( fileName ) == true )
		{
			for( TSettingsMap::iterator it = m_settings.begin(), it_end = m_settings.end();
				it != it_end;
				it++ )
				{
					it->second.first = config.getSetting( it->first.c_str(), "SETTINGS" );
				}
		}
		else
		{
			MENGE_LOG_ERROR( "Parsing Account settings failed \"%s\""
				, fileName.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::save()
	{
		FileEngine* fileEngine = Holder<FileEngine>::hostage();
		String fileName = m_name + "/settings.ini";
		OutStreamInterface* file = fileEngine->openOutStream( fileName, false );
		if( file == 0 )
		{
			MENGE_LOG_ERROR( "can't open file for writing. Account \"%s\" settings not saved"
				, m_name.c_str() );
			return;
		}
		file->write( "[SETTINGS]\n" );
		for( TSettingsMap::iterator it = m_settings.begin(), it_end = m_settings.end();
			it != it_end;
			it++ )
		{
			file->write( it->first + "\t= " + it->second.first + "\n" );
		}

		fileEngine->closeOutStream( file );
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::loader_( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			for( TSettingsMap::iterator it = m_settings.begin(), it_end = m_settings.end();
				it != it_end;
				it++ )
			{
				XML_CASE_ATTRIBUTE_NODE( it->first.c_str(), "Value", it->second.first );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::apply()
	{
		for( TSettingsMap::iterator it = m_settings.begin(), it_end = m_settings.end();
			it != it_end;
			it++ )
		{
			pybind::call( it->second.second, "(ss)", it->first.c_str(), it->second.first.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}