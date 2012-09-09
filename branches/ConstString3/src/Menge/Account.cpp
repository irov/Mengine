#	include "Account.h"

#	include "FileEngine.h"
#	include "ConfigFile/ConfigFile.h"

#	include "Consts.h"

#	include "LogEngine.h"

#	include "Core/String.h"
#	include "Core/File.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Account::Account( const WString & _name )
		: m_name(_name)
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
			pybind::decref( it->second.second );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Account::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc )
	{
		TMapSettings::iterator it = m_settings.find( _setting );
		
		if( it != m_settings.end() )
		{
			MENGE_LOG_ERROR( "Warning: Setting '%S' already exist"
				, _setting.c_str() 
				);

			return;
		}

		pybind::incref( _applyFunc );

		m_settings[_setting] = std::make_pair( _defaultValue, _applyFunc );
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::changeSetting( const WString& _setting, const WString& _value )
	{
		TMapSettings::iterator it = m_settings.find( _setting );
		
		if( it == m_settings.end() )
		{
			MENGE_LOG_ERROR( "Account::changeSetting setting '%S' does not exist. Can't change"
				, _setting.c_str()
				);

			return;
		}

		it->second.first = _value;

		if( pybind::is_none( it->second.second ) == false )
		{
			pybind::call( it->second.second, "(u)", _value.c_str() );
		}		

		this->save();
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Account::getSetting( const WString& _setting ) const
	{
		TMapSettings::const_iterator it = m_settings.find( _setting );
		
		if( it == m_settings.end() )
		{
			MENGE_LOG_ERROR( "setting '%S' does not exist. Can't get"
				, _setting.c_str()
				);

			return Utils::emptyWString();
		}

		return it->second.first;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Account::load()
	{
		ConfigFile config;

		WString fileName = m_name + MENGE_FOLDER_DELIM + L"settings.ini";

        if( FileEngine::get()
            ->existFile( Consts::get()->c_user, fileName ) == false )
        {
            MENGE_LOG_ERROR( "Account settings not found '%S'"
                , fileName.c_str() 
                );

            return false;
        }

		FileInputStreamInterface* file = FileEngine::get()
			->openInputFile( Consts::get()->c_user, fileName );

		if( config.load( file ) == false )
		{
			MENGE_LOG_ERROR( "Parsing Account settings failed '%S'"
				, fileName.c_str() 
				);

			return false;
		}

		file->close();

		for( TMapSettings::iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		it++ )
		{
			config.getSetting( L"SETTINGS", it->first.c_str(), it->second.first );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::save()
	{
		WString fileName = m_name + MENGE_FOLDER_DELIM + L"settings.ini";

		FileOutputStreamInterface* file = FileEngine::get()
			->openOutputFile( Consts::get()->c_user, fileName );

		if( file == 0 )
		{
			MENGE_LOG_ERROR( "Account::save can't open file for writing. Account '%S' settings not saved"
				, m_name.c_str() 
				);

			return;
		}

		ConfigFile config;
				
		//Utils::stringWriteU( file, L"[SETTINGS]\n" );

		for( TMapSettings::iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		it++ )
		{
            const WString & key = it->first;
            const WString & value = it->second.first;

			config.setSetting( L"SETTINGS", key, value );
			//Utils::stringWriteU( file, it->first + L" = " + it->second.first + L"\n" );
		}

		config.save( file );

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

			pybind::call( it->second.second, "(u)", it->second.first.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
