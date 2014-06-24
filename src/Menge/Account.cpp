#	include "Account.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Consts.h"

#   include "Logger/Logger.h"

#	include "Core/CacheMemoryBuffer.h"
#	include "Core/IniUtil.h"
#	include "Core/String.h"
#	include "Core/File.h"
#	include "Core/CRC32.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Account::Account()
		: m_serviceProvider(nullptr)
		, m_projectVersion(0)
		, m_projectVersionCheck(true)
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
            Setting & st = m_settings.get_value( it );

			pybind::decref( st.cb );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Account::initialize( ServiceProviderInterface * _serviceProvider, const WString & _name, const FilePath & _folder, size_t _projectVersion, bool _projectVersionCheck )
	{
		m_serviceProvider = _serviceProvider;
		m_name = _name;
		m_projectVersion = _projectVersion;
		m_projectVersionCheck = _projectVersionCheck;

		m_folder = _folder;

		PathString settingsPath;
		settingsPath.append( m_folder );
		settingsPath.append( MENGE_FOLDER_DELIM );
		settingsPath.append( "settings.ini" );

		m_settingsPath = Helper::stringizeString( m_serviceProvider, settingsPath );

		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "zip") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
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
	bool Account::addSetting( const ConstString & _setting, const WString& _defaultValue, PyObject* _applyFunc )
	{
		TMapSettings::iterator it = m_settings.find( _setting );
		
		if( it != m_settings.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::addSetting: account '%ls' setting '%s' already exist"
                , m_name.c_str()
				, _setting.c_str() 
				);

			return false;
		}

		pybind::incref( _applyFunc );

        Setting st;
        st.value = _defaultValue;
        st.cb = _applyFunc;
        
        m_settings.insert( _setting, st );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Account::changeSetting( const ConstString & _setting, const WString& _value )
	{
		Setting * st = nullptr;		
		if( m_settings.has( _setting, &st ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::changeSetting account %ls setting '%s' does not exist. Can't change"
                , m_name.c_str()
				, _setting.c_str()
				);

			return false;
		}

		st->value = _value;

		if( pybind::is_none( st->cb ) == false )
		{
			pybind::call( st->cb, "(O)"
                , pybind::ptr(_value)
                );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Account::getSetting( const ConstString & _setting ) const
	{
		const Setting * st;
		if( m_settings.has( _setting, &st ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::getSetting account '%ls' setting '%s' does not exist. Can't get"
                , m_name.c_str()
				, _setting.c_str()
				);

			return Utils::emptyWString();
		}

		return st->value;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Account::hasSetting( const ConstString & _setting ) const
    {
		bool result = m_settings.exist( _setting );

        return result;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Account::load()
	{
        if( FILE_SERVICE(m_serviceProvider)
			->existFile( CONST_STRING_LOCAL(m_serviceProvider, "user"), m_settingsPath, nullptr ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::load account '%ls' settings not found '%s'"
                , m_name.c_str()
                , m_settingsPath.c_str() 
                );

            return false;
        }

		InputStreamInterfacePtr file = FILE_SERVICE(m_serviceProvider)
			->openInputFile( CONST_STRING_LOCAL(m_serviceProvider, "user"), m_settingsPath );

        if( file == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::load can't open file for read. Account '%ls' settings not load '%s'"
                , m_name.c_str() 
                , m_settingsPath.c_str()
                );

            return false;
        }

		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, file, m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::load parsing Account '%ls' settings failed '%s'"
                , m_name.c_str()
				, m_settingsPath.c_str() 
				);

			return false;
		}

        file = nullptr;

		if( m_projectVersionCheck == true )
		{
			String projectVersion_s;
			if( IniUtil::getIniValue( ini, "PROJECT", "VERSION", projectVersion_s, m_serviceProvider ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Account::load account '%ls' failed not found project version"
					, m_name.c_str()
					);

				return false;
			}

			size_t projectVersion = 0;
			if( Utils::stringToUnsigned( projectVersion_s, projectVersion ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Account::load account '%ls' failed invalid project version '%s'"
					, m_name.c_str()
					, projectVersion_s.c_str()
					);

				return false;
			}

			if( m_projectVersion != projectVersion )
			{
				LOGGER_ERROR(m_serviceProvider)("Account::load account '%ls' failed invalid project version '%d' need '%d'"
					, m_name.c_str()
					, projectVersion
					, m_projectVersion
					);

				return false;
			}
		}

		for( TMapSettings::iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		++it )
		{
            const ConstString & key = m_settings.get_key(it);
            Setting & st = m_settings.get_value(it);

            if( IniUtil::getIniValue( ini, "SETTINGS", key.c_str(), st.value, m_serviceProvider ) == false )
            {
                LOGGER_WARNING(m_serviceProvider)("Account::load account '%ls' failed get setting '%s'"
                    , m_name.c_str()
                    , key.c_str() 
                    );
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
			LOGGER_ERROR(m_serviceProvider)("Account::save can't open file for writing. Account '%ls' settings not saved %s"
				, m_name.c_str() 
                , m_settingsPath.c_str()
				);

			return false;
		}

		IniUtil::writeIniSection( m_serviceProvider, file, "[PROJECT]" );

		WString projectVersion_s;
		if( Utils::unsignedToWString( m_projectVersion, projectVersion_s ) == false )
		{
			return false;
		}

		IniUtil::writeIniSetting( m_serviceProvider, file, "VERSION", projectVersion_s );
        
        IniUtil::writeIniSection( m_serviceProvider, file, "[SETTINGS]" );
        
		for( TMapSettings::const_iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		++it )
		{
            const ConstString & key = m_settings.get_key(it);
            const Setting & st = m_settings.get_value(it);

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
            Setting & st = m_settings.get_value(it);
           
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
    bool Account::loadBinaryFile( const FilePath & _fileName, TBlobject & _data )
    {        
		PathString path;

		path += m_folder;
		path += MENGE_FOLDER_DELIM;
		path += _fileName;

		ConstString fullpath = Helper::stringizeString( m_serviceProvider, path );

        InputStreamInterfacePtr stream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( CONST_STRING(m_serviceProvider, user), fullpath );

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::loadBinaryFile: account %ls invalid load file %s (file not found)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }

        size_t file_size = stream->size();

        uint32_t load_crc32 = 0;
        if( stream->read( &load_crc32, sizeof(load_crc32) ) != sizeof(load_crc32) )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::loadBinaryFile: account %ls invalid load file %s (load crc32)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }

        uint32_t load_data_size = 0;
        if( stream->read( &load_data_size, sizeof(load_data_size) ) != sizeof(load_data_size) )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::loadBinaryFile: account %ls invalid load file %s (load data size)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }

        size_t load_compress_size = file_size - sizeof(load_crc32) - sizeof(load_data_size);

		CacheMemoryBuffer data_buffer(m_serviceProvider, load_compress_size, "Account_loadBinaryFile");
		TBlobject::value_type * data_memory = data_buffer.getMemoryT<TBlobject::value_type>();
        
		stream->read( data_memory, load_compress_size );

        stream = nullptr;

        size_t check_crc32 = make_crc32( data_memory, load_compress_size );

        if( load_crc32 != check_crc32 )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::loadBinaryFile: account %ls: invalid load file %s (crc32 incorect)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }

        _data.resize( load_data_size );

        size_t uncompress_size;
        if( m_archivator->decompress( &_data[0], load_data_size, data_memory, load_compress_size, uncompress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::loadBinaryFile: account %ls: invalid load file %s (uncompress failed)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::writeBinaryFile( const FilePath & _fileName, const TBlobject & _data )
    {
		PathString path;
		
		path += m_folder;
		path += MENGE_FOLDER_DELIM;
		path += _fileName;

		ConstString fullpath = Helper::stringizeString( m_serviceProvider, path );

        OutputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( CONST_STRING(m_serviceProvider, user), fullpath );

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls invalid write file %s (not create)"
                , m_name.c_str()
                , _fileName.c_str()
                );

            return false;
        }

        if( _data.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls write empty file %s"
                , m_name.c_str()
                , _fileName.c_str()
                );

            return false;
        }

        uint32_t data_size = _data.size();

		MemoryInputPtr compress_memory = ARCHIVE_SERVICE(m_serviceProvider)
			->compressBuffer( m_archivator, &_data[0], data_size );
        
        if( compress_memory == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls invalid compress file %s"
                , m_name.c_str()
                , _fileName.c_str()
                );

            return false;
        }

		uint32_t compressSize;
		const void * compressBuffer = compress_memory->getMemory( compressSize );

        uint32_t value_crc32 = make_crc32( (const unsigned char *)compressBuffer, compressSize );

        if( stream->write( &value_crc32, sizeof(value_crc32) ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls invalid write 'crc32' %s (not create)"
                , m_name.c_str()
                , _fileName.c_str()
                );

            return false;
        }

        if( stream->write( &data_size, sizeof(data_size) ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls invalid write 'data size' %s (not create)"
                , m_name.c_str()
                , _fileName.c_str()
                );

            return false;
        }
                
        if( stream->write( compressBuffer, compressSize ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls invalid write 'data' %s (not create)"
                , m_name.c_str()
                , _fileName.c_str()
                );

            return false;
        }

        return true;
    }
}
