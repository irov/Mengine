#	include "Account.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "ConfigFile/ConfigFile.h"

#	include "Consts.h"

#   include "Logger/Logger.h"

#	include "Core/String.h"
#	include "Core/File.h"
#	include "Core/CRC32.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Account::Account( ServiceProviderInterface * _serviceProvider, const WString & _name )
		: m_serviceProvider(_serviceProvider)
        , m_name(_name)
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
            Setting & st = it->second;

			pybind::decref( st.cb );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Account::setFolder( const FilePath & _folder )
    {
        m_folder = _folder;

        String settingsPath;
        settingsPath.append( m_folder.c_str(), m_folder.size() ); 
        settingsPath += MENGE_FOLDER_DELIM;
        settingsPath += "settings.ini";

        m_settingsPath = Helper::stringizeString( m_serviceProvider, settingsPath );
    }
	//////////////////////////////////////////////////////////////////////////
	const WString & Account::getName() const
	{
		return m_name;
	}
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Account::getFolder() const
    {
        return m_folder;
    }
	//////////////////////////////////////////////////////////////////////////
	void Account::addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc )
	{
		TMapSettings::iterator it = m_settings.find( _setting );
		
		if( it != m_settings.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "Warning: Setting '%ls' already exist"
				, _setting.c_str() 
				);

			return;
		}

		pybind::incref( _applyFunc );

        Setting st;
        st.value = _defaultValue;
        st.cb = _applyFunc;
        
		m_settings[_setting] = st;
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::changeSetting( const WString& _setting, const WString& _value )
	{
		TMapSettings::iterator it = m_settings.find( _setting );
		
		if( it == m_settings.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "Account::changeSetting setting '%ls' does not exist. Can't change"
				, _setting.c_str()
				);

			return;
		}

        Setting & st = it->second;

		st.value = _value;

		if( pybind::is_none( st.cb ) == false )
		{
			pybind::call( st.cb, "(O)"
                , pybind::ptr(_value)
                );
		}		

		this->save();
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Account::getSetting( const WString& _setting ) const
	{
		TMapSettings::const_iterator it = m_settings.find( _setting );
		
		if( it == m_settings.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "Account::getSetting '%ls' does not exist. Can't get"
				, _setting.c_str()
				);

			return Utils::emptyWString();
		}

        const Setting & st = it->second;

		return st.value;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Account::hasSetting( const WString& _setting ) const
    {
        TMapSettings::const_iterator it = m_settings.find( _setting );

        if( it == m_settings.end() )
        {
            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Account::load()
	{
		ConfigFile config;

        if( FILE_SERVICE(m_serviceProvider)->existFile( CONST_STRING(m_serviceProvider, user), m_settingsPath, NULL ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::load settings not found '%s'"
                , m_settingsPath.c_str() 
                );

            return false;
        }

		InputStreamInterface* file = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( CONST_STRING(m_serviceProvider, user), m_settingsPath );

        if( file == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::load can't open file for read. Account '%ls' settings not load '%s'"
                , m_name.c_str() 
                , m_settingsPath.c_str()
                );

            return false;
        }

		if( config.load( file ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Account::load parsing Account settings failed '%s'"
				, m_settingsPath.c_str() 
				);

            file->destroy();

			return false;
		}

		file->destroy();
        file = NULL;

		for( TMapSettings::iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		++it )
		{
            const WString & key = it->first;

            Setting & st = it->second;

			if( config.getSetting( L"SETTINGS", key, st.value ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "Account::load failed get setting '%ls'"
                    , key.c_str() 
                    );

                return false;
            }
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Account::save()
	{
		OutputStreamInterface* file = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( CONST_STRING(m_serviceProvider, user), m_settingsPath );

		if( file == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "Account::save can't open file for writing. Account '%ls' settings not saved %s"
				, m_name.c_str() 
                , m_settingsPath.c_str()
				);

			return false;
		}

		ConfigFile config;
				
		//Utils::stringWriteU( file, L"[SETTINGS]\n" );

		for( TMapSettings::iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		++it )
		{
            const WString & key = it->first;

            Setting & st = it->second;

			config.setSetting( L"SETTINGS", key, st.value );
			//Utils::stringWriteU( file, it->first + L" = " + it->second.first + L"\n" );
		}

		if( config.save( file ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::save can't save config. Account '%ls' settings not saved %s"
                , m_name.c_str() 
                , m_settingsPath.c_str()
                );

            file->destroy();
            file = NULL;

            return false;
        }

		file->destroy();
        file = NULL;

        return true;
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
            Setting & st = it->second;
           
			if( pybind::is_none(st.cb) == true )
			{
				continue;
			}

			pybind::call( st.cb, "(O)"
                , pybind::ptr(st.value)
                );
		}
	}
	//////////////////////////////////////////////////////////////////////////
    bool Account::loadBinaryFile( const FilePath & _filename, TBlobject & _data )
    {
        String cachepath;
        cachepath.append( m_folder.c_str(), m_folder.size() );
        cachepath += MENGE_FOLDER_DELIM;
        cachepath.append( _filename.c_str(), _filename.size() );

        ConstString fullpath = Helper::stringizeString( m_serviceProvider, cachepath );

        InputStreamInterface * file = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( CONST_STRING(m_serviceProvider, user), fullpath );

        if( file == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::loadBinaryFile: account %ls invalid load file %s (file not found)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }

        size_t file_size = file->size();

        size_t load_crc32 = 0;

        size_t data_size = file_size - sizeof(load_crc32);

        _data.resize( data_size + 1 );
        _data[data_size] = 0;

        file->read( &load_crc32, sizeof(load_crc32) );
        file->read( &_data[0], data_size );

        file->destroy();

        size_t check_crc32 = make_crc32( &_data[0], data_size );

        if( load_crc32 != check_crc32 )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::loadBinaryFile: account %ls: invalid load file %s (crc32 incorect)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }		

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::writeBinaryFile( const FilePath & _filename, const TBlobject & _data )
    {
        String cachepath;
        cachepath.append( m_folder.c_str(), m_folder.size() );
        cachepath += MENGE_FOLDER_DELIM;
        cachepath.append( _filename.c_str(), _filename.size() );

        ConstString fullpath = Helper::stringizeString( m_serviceProvider, cachepath );

        OutputStreamInterface * file = FILE_SERVICE(m_serviceProvider)->openOutputFile( CONST_STRING(m_serviceProvider, user), fullpath );

        if( file == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls invalid write file %s (not create)"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }

        size_t value_crc32 = make_crc32( &_data[0], _data.size() );

        if( file->write( &value_crc32, sizeof(value_crc32) ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls invalid write crc32 %s (not create)"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }
        
        if( file->write( &_data[0], _data.size() ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls invalid write data %s (not create)"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }

        file->destroy();

        return true;
    }
}
