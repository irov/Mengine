#	include "Account.h"

#	include "FileEngine.h"
#	include "ConfigFile.h"

#	include "Consts.h"

#	include "BinParser.h"

#	include "LogEngine.h"

#	include "Core/String.h"
#	include "Core/File.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Account::Account( const String& _folder )
		: m_folder( _folder )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Account::~Account()
	{
		for( TMapSettings::iterator
			it = m_settings.begin(),
			it_end = m_settings.end();
		it != it_end;
		++it )
		{
			pybind::decref(it->second.second);
		}

		for( TMapSettings::iterator
			it = m_settingsU.begin(),
			it_end = m_settingsU.end();
		it != it_end;
		++it )
		{
			pybind::decref(it->second.second);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Account::getFolder() const
	{
		return m_folder;
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::addSetting( const String& _setting, const String& _defaultValue, PyObject* _applyFunc )
	{
		TMapSettings::iterator it = m_settings.find( _setting );
		
		if( it != m_settings.end() )
		{
			MENGE_LOG_ERROR( "Warning: Setting '%s' already exist"
				, _setting.c_str() 
				);

			return;
		}

		pybind::incref( _applyFunc );

		m_settings[_setting] = std::make_pair( _defaultValue, _applyFunc );
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::changeSetting( const String& _setting, const String& _value )
	{
		TMapSettings::iterator it = m_settings.find( _setting );
		
		if( it == m_settings.end() )
		{
			MENGE_LOG_ERROR( "setting '%s' does not exist. Can't change"
				, _setting.c_str()
				);

			return;
		}

		it->second.first = _value;

		if( pybind::is_none(it->second.second) == false )
		{
			pybind::call( it->second.second, "(s)", _value.c_str() );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Account::getSetting( const String& _setting )
	{
		TMapSettings::iterator it = m_settings.find( _setting );
		
		if( it == m_settings.end() )
		{
			MENGE_LOG_ERROR( "setting '%s' does not exist. Can't get"
				, _setting.c_str()
				);

			return Utils::emptyString();
		}

		return it->second.first;
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::addSettingU( const String& _setting, const String& _defaultValue, PyObject* _applyFunc )
	{
		TMapSettings::iterator it = m_settingsU.find( _setting );

		if( it != m_settingsU.end() )
		{
			MENGE_LOG_ERROR( "Warning: setting U '%s' already exist"
				, _setting.c_str() 
				);

			return;
		}

		pybind::incref( _applyFunc );

		m_settingsU[_setting] = std::make_pair( _defaultValue, _applyFunc );
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::changeSettingU( const String& _setting, const String& _value )
	{
		TMapSettings::iterator it = m_settingsU.find( _setting );

		if( it == m_settingsU.end() )
		{
			MENGE_LOG_ERROR( "setting U '%s' does not exist. Can't change"
				, _setting.c_str()
				);

			return;
		}

		it->second.first = _value;

		if( pybind::is_none(it->second.second) == false )
		{
			PyObject* uValue = pybind::unicode_from_utf8( _value.c_str(), _value.length() );
			pybind::call( it->second.second, "(O)", uValue );
			pybind::decref(uValue);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Account::getSettingU( const String& _setting )
	{
		TMapSettings::iterator it = m_settingsU.find( _setting );

		if( it == m_settingsU.end() )
		{
			MENGE_LOG_ERROR( "setting '%s' does not exist. Can't get"
				, _setting.c_str()
				);

			return Utils::emptyString();
		}

		return it->second.first;
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::load()
	{
		String fileName = Helper::to_str(m_folder) + "/settings.ini";
		ConfigFile config;

		if( config.load( Consts::get()->c_user, fileName ) == false )
		{
			MENGE_LOG_ERROR( "Parsing Account settings failed '%s'"
				, fileName.c_str() 
				);

			return;
		}

		for( TMapSettings::iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		it++ )
		{
			config.getSetting( it->first.c_str(), "SETTINGS", it->second.first );
		}

		for( TMapSettings::iterator 
			it = m_settingsU.begin(), 
			it_end = m_settingsU.end();
		it != it_end;
		it++ )
		{
			config.getSetting( it->first.c_str(), "SETTINGSU", it->second.first );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::save()
	{
		FileEngine* fileEngine = FileEngine::get();
		
		String fileName = Helper::to_str(m_folder) + "/settings.ini";
		FileOutputInterface* file = fileEngine->openOutputFile( Consts::get()->c_user, fileName );

		if( file == 0 )
		{
			MENGE_LOG_ERROR( "can't open file for writing. Account '%s' settings not saved"
				, m_folder.c_str() 
				);

			return;
		}

		Utils::stringWrite( file, "[SETTINGS]\n" );

		for( TMapSettings::iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		it++ )
		{
			Utils::stringWrite( file, it->first + " = " + it->second.first + "\n" );
		}

		Utils::stringWrite( file, "[SETTINGSU]\n" );
		
		for( TMapSettings::iterator 
			it = m_settingsU.begin(), 
			it_end = m_settingsU.end();
		it != it_end;
		it++ )
		{
			Utils::stringWrite( file, it->first + " = " + it->second.first + "\n" );
		}

		file->close();
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::apply()
	{
		for( TMapSettings::iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		++it )
		{
			if( pybind::is_none(it->second.second) == true )
			{
				continue;
			}

			PyObject* py_value = pybind::string_from_char( it->second.first.c_str(), it->second.first.length() );
			pybind::call( it->second.second, "(O)", py_value );
			pybind::decref(py_value);
		}

		for( TMapSettings::iterator 
			it = m_settingsU.begin(), 
			it_end = m_settingsU.end();
		it != it_end;
		++it )
		{
			if( pybind::is_none(it->second.second) == true )
			{
				continue;
			}

			PyObject* py_valueU = pybind::unicode_from_utf8( it->second.first.c_str(), it->second.first.length() );
			pybind::call( it->second.second, "(O)", py_valueU );
			pybind::decref(py_valueU);
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
