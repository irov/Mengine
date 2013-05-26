#	include "Account.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Consts.h"

#   include "Logger/Logger.h"

#	include "Core/IniUtil.h"

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
	void Account::addSetting( const ConstString & _setting, const WString& _defaultValue, PyObject* _applyFunc )
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
        
        m_settings.insert( std::make_pair(_setting, st) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::changeSetting( const ConstString & _setting, const WString& _value )
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
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Account::getSetting( const ConstString & _setting ) const
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
    bool Account::hasSetting( const ConstString & _setting ) const
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
        if( FILE_SERVICE(m_serviceProvider)->existFile( CONST_STRING(m_serviceProvider, user), m_settingsPath, nullptr ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::load settings not found '%s'"
                , m_settingsPath.c_str() 
                );

            return false;
        }

		InputStreamInterfacePtr file = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( CONST_STRING(m_serviceProvider, user), m_settingsPath );

        if( file == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::load can't open file for read. Account '%ls' settings not load '%s'"
                , m_name.c_str() 
                , m_settingsPath.c_str()
                );

            return false;
        }

        Ini ini(m_serviceProvider);
		if( ini.load( file ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Account::load parsing Account settings failed '%s'"
				, m_settingsPath.c_str() 
				);

			return false;
		}

        file = nullptr;

		for( TMapSettings::iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		++it )
		{
            const ConstString & key = it->first;

            Setting & st = it->second;

            if( IniUtil::getIniValue( ini, "SETTINGS", key.c_str(), st.value, m_serviceProvider ) == false )
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
		OutputStreamInterfacePtr file = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( CONST_STRING(m_serviceProvider, user), m_settingsPath );

		if( file == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "Account::save can't open file for writing. Account '%ls' settings not saved %s"
				, m_name.c_str() 
                , m_settingsPath.c_str()
				);

			return false;
		}
        
        IniUtil::writeIniSection( m_serviceProvider, file, "[SETTINGS]" );
        
		for( TMapSettings::const_iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		++it )
		{
            const ConstString & key = it->first;

            const Setting & st = it->second;

            IniUtil::writeIniSetting( m_serviceProvider, file, key.c_str(), st.value );
		}

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
        static String cachepath;
        cachepath.assign( m_folder.c_str(), m_folder.size() );
        cachepath += MENGE_FOLDER_DELIM;
        cachepath.append( _filename.c_str(), _filename.size() );

        ConstString fullpath = Helper::stringizeString( m_serviceProvider, cachepath );

        InputStreamInterfacePtr file = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( CONST_STRING(m_serviceProvider, user), fullpath );

        if( file == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::loadBinaryFile: account %ls invalid load file %s (file not found)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }

        //size_t file_size = file->size();

        size_t load_crc32 = 0;
        if( file->read( &load_crc32, sizeof(load_crc32) ) != sizeof(load_crc32) )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::loadBinaryFile: account %ls invalid load file %s (load crc32)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }

        size_t load_data_size = 0;
        if( file->read( &load_data_size, sizeof(load_data_size) ) != sizeof(load_data_size) )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::loadBinaryFile: account %ls invalid load file %s (load data size)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }

        size_t load_compress_size = 0;
        if( file->read( &load_compress_size, sizeof(load_compress_size) ) != sizeof(load_compress_size) )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::loadBinaryFile: account %ls invalid load file %s (load compress size)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }
        //size_t data_size = file_size - sizeof(load_crc32);

        TBlobject archive_blob(load_compress_size);
        file->read( &archive_blob[0], load_compress_size );
        file = nullptr;

        size_t check_crc32 = make_crc32( &archive_blob[0], load_compress_size );

        if( load_crc32 != check_crc32 )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::loadBinaryFile: account %ls: invalid load file %s (crc32 incorect)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }

        _data.resize( load_data_size );

        size_t uncompress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->uncompress( &_data[0], load_data_size, uncompress_size, &archive_blob[0], load_compress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::loadBinaryFile: account %ls: invalid load file %s (uncompress failed)"
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
        static String cachepath;
        cachepath.assign( m_folder.c_str(), m_folder.size() );
        cachepath += MENGE_FOLDER_DELIM;
        cachepath.append( _filename.c_str(), _filename.size() );

        ConstString fullpath = Helper::stringizeString( m_serviceProvider, cachepath );

        OutputStreamInterfacePtr file = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( CONST_STRING(m_serviceProvider, user), fullpath );

        if( file == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls invalid write file %s (not create)"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }

        if( _data.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls write empty file %s"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }

        size_t data_size = _data.size();
        
        size_t archive_size = ARCHIVE_SERVICE(m_serviceProvider)
            ->compressBound( data_size );

        if( archive_size == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls invalid write file %s (archive_size is zero)"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }

        TBlobject archive_blob(archive_size);

        size_t comress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->compress( &archive_blob[0], archive_size, comress_size, &_data[0], _data.size() ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls invalid write 'crc32' %s (compress failed)"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }

        size_t value_crc32 = make_crc32( &archive_blob[0], comress_size );

        if( file->write( &value_crc32, sizeof(value_crc32) ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls invalid write 'crc32' %s (not create)"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }

        if( file->write( &data_size, sizeof(data_size) ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls invalid write 'data size' %s (not create)"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }

        if( file->write( &comress_size, sizeof(comress_size) ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls invalid write 'compress size' %s (not create)"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }
        
        if( file->write( &archive_blob[0], comress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Account::writeBinaryFile: account %ls invalid write 'data' %s (not create)"
                , m_name.c_str()
                , _filename.c_str()
                );

            return false;
        }

        return true;
    }
}
