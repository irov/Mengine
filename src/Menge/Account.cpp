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
	Account::Account( /*const String& _name,*/ const String& _folder )
		: /*m_name( _name )
		,*/ m_folder( _folder )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Account::~Account()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	/*const String& Account::getName() const
	{
		return m_name;
	}*/
	//////////////////////////////////////////////////////////////////////////
	const String& Account::getFolder() const
	{
		return m_folder;
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
			MENGE_LOG_ERROR( "Warning: Setting '%s' already exist"
				, _setting.c_str() 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::changeSetting( const String& _setting, const String& _value )
	{
		TSettingsMap::iterator it = m_settings.find( _setting );
		if( it != m_settings.end() )
		{
			PyObject* uSetting = PyUnicode_DecodeUTF8( _setting.c_str(), _setting.length(), NULL );
			PyObject* uValue = PyUnicode_DecodeUTF8( _value.c_str(), _value.length(), NULL );
			pybind::call( it->second.second, "(OO)", uSetting, uValue );

			//pybind::call( it->second.second, "(ss)", _setting.c_str(), _value.c_str() );
			it->second.first = _value;
		}
		else
		{
			MENGE_LOG_ERROR( "setting '%s' does not exist. Can't change"
				, _setting.c_str()
				);
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
			MENGE_LOG_ERROR( "setting '%s' does not exist. Can't get"
				, _setting.c_str()
				);
		}

		return Utils::emptyString();
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::load()
	{
		String fileName = m_folder + "/settings.ini";
		ConfigFile config;
		if( config.load( "user", fileName ) == true )
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
			MENGE_LOG_ERROR( "Parsing Account settings failed '%s'"
				, fileName.c_str() 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::save()
	{
		FileEngine* fileEngine = FileEngine::hostage();
		String fileName = m_folder + "/settings.ini";
		FileOutput* file = fileEngine->openFileOutput( "user", fileName );
		if( file == 0 )
		{
			MENGE_LOG_ERROR( "can't open file for writing. Account '%s' settings not saved"
				, m_folder.c_str() );
			return;
		}
		Utils::fileWrite( file, "[SETTINGS]\n" );
		for( TSettingsMap::iterator it = m_settings.begin(), it_end = m_settings.end();
			it != it_end;
			it++ )
		{
			Utils::fileWrite( file, it->first + "\t= " + it->second.first + "\n" );
		}

		fileEngine->closeFileOutput( file );
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
			//const char* key = it->first.c_str();
			//const char* value = it->second.first.c_str();
			PyObject* uKey = PyUnicode_DecodeUTF8( it->first.c_str(), it->first.length(), NULL );
			PyObject* uValue = PyUnicode_DecodeUTF8( it->second.first.c_str(), it->second.first.length(), NULL );
			pybind::call( it->second.second, "(OO)", uKey, uValue );
			Py_DECREF(uKey);
			Py_DECREF(uValue);
			//String keyAnsi = Holder<Application>::hostage()->utf8ToAnsi( it->first );
			//String valueAnsi = Holder<Application>::hostage()->utf8ToAnsi( it->second.first );
			//pybind::call( it->second.second, "(ss)", keyAnsi.c_str(), valueAnsi.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
