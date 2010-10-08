#	include "Account.h"

#	include "FileEngine.h"
#	include "ConfigFile.h"

#	include "Consts.h"

#	include "BinParser.h"

#	include "Logger/Logger.h"

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
		TMapSettings::iterator it = m_settings.find( _setting );
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
		TMapSettings::iterator it = m_settings.find( _setting );
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
		String fileName = m_folder.str() + "/settings.ini";

		ConfigFile config;
		if( config.load( Consts::get()->c_user, fileName ) == true )
		{
			for( TMapSettings::iterator it = m_settings.begin(), it_end = m_settings.end();
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
		FileEngine* fileEngine = FileEngine::get();
		
		String fileName = m_folder.str() + "/settings.ini";
		FileOutputInterface* file = fileEngine->openOutputFile( Consts::get()->c_user, fileName );

		if( file == 0 )
		{
			MENGE_LOG_ERROR( "can't open file for writing. Account '%s' settings not saved"
				, m_folder.c_str() );
			return;
		}

		Utils::stringWrite( file, "[SETTINGS]\n" );

		for( TMapSettings::iterator it = m_settings.begin(), it_end = m_settings.end();
			it != it_end;
			it++ )
		{
			Utils::stringWrite( file, it->first + "\t= " + it->second.first + "\n" );
		}

		fileEngine->closeOutputFile( file );
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::loader( BinParser * _parser )
	{
		//BIN_SWITCH_ID( _parser )
		//{
		//	for( TMapSettings::iterator 
		//		it = m_settings.begin(), 
		//		it_end = m_settings.end();
		//	it != it_end;
		//	it++ )
		//	{
		//		//BIN_CASE_USER_VALUE(  )
		//		//XML_CASE_ATTRIBUTE_NODE( it->first.c_str(), "Value", it->second.first );
		//	}
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::loaded()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::apply()
	{
		for( TMapSettings::iterator it = m_settings.begin(), it_end = m_settings.end();
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
			//String keyAnsi = Holder<Application>::get()->utf8ToAnsi( it->first );
			//String valueAnsi = Holder<Application>::get()->utf8ToAnsi( it->second.first );
			//pybind::call( it->second.second, "(ss)", keyAnsi.c_str(), valueAnsi.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
