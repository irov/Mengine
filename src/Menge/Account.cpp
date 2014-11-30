#	include "Account.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/CacheInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Consts.h"

#   include "Logger/Logger.h"

#	include "Core/CacheMemoryBuffer.h"
#	include "Core/IniUtil.h"
#	include "Core/String.h"
#	include "Core/Stream.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Account::Account()
		: m_serviceProvider(nullptr)
		, m_projectVersion(0)
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
	bool Account::initialize( ServiceProviderInterface * _serviceProvider, const WString & _name, const ConstString & _folder, uint32_t _projectVersion )
	{
		m_serviceProvider = _serviceProvider;
		m_name = _name;
		m_projectVersion = _projectVersion;

		m_folder = _folder;

		PathString settingsPath;
		settingsPath.append( m_folder );
		settingsPath.append( MENGE_FOLDER_DELIM );
		settingsPath.append( "settings.ini" );

		m_settingsPath = Helper::stringizeString( m_serviceProvider, settingsPath );

		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( STRINGIZE_STRING_LOCAL(m_serviceProvider, "lz4") );

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
    const ConstString & Account::getFolder() const
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
			->existFile( STRINGIZE_STRING_LOCAL(m_serviceProvider, "user"), m_settingsPath, nullptr ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::load account '%ls' settings not found '%s'"
                , m_name.c_str()
                , m_settingsPath.c_str() 
                );

            return false;
        }

		InputStreamInterfacePtr file = FILE_SERVICE(m_serviceProvider)
			->openInputFile( STRINGIZE_STRING_LOCAL(m_serviceProvider, "user"), m_settingsPath, false );

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

		String projectVersion_s;
		if( IniUtil::getIniValue( ini, "PROJECT", "VERSION", projectVersion_s, m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::load account '%ls' failed not found project version"
				, m_name.c_str()
				);

			return false;
		}

		uint32_t projectVersion = 0;
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
    CacheBufferID Account::loadBinaryFile( const ConstString & _fileName, const void ** _data, size_t & _size )
    {        
		PathString path;

		path += m_folder;
		path += MENGE_FOLDER_DELIM;
		path += _fileName;

		ConstString fullpath = Helper::stringizeString( m_serviceProvider, path );

        InputStreamInterfacePtr stream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( CONST_STRING(m_serviceProvider, user), fullpath, false );

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::loadBinaryFile: account %ls invalid load file %s (file not found)"
                , m_name.c_str()
                , fullpath.c_str()
                );

            return 0;
        }
		
		CacheBufferID bufferId;
		unsigned char * data_memory;
		size_t data_size;
		
		if( Helper::loadStreamArchiveData( m_serviceProvider, stream, m_archivator, GET_MAGIC_NUMBER(MAGIC_ACCOUNT_DATA), GET_MAGIC_VERSION(MAGIC_ACCOUNT_DATA), bufferId, &data_memory, data_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::loadBinaryFile: account %ls invalid load file %s"
				, m_name.c_str()
				, fullpath.c_str()
				);

			return false;
		}

		*_data = data_memory;
		_size = data_size;

        return bufferId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::writeBinaryFile( const ConstString & _fileName, const void * _data, size_t _size )
    {
		if( _data == nullptr || _size == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls write empty file %s"
				, m_name.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		PathString path;
		
		path += m_folder;
		path += MENGE_FOLDER_DELIM;
		path += _fileName;

		ConstString fullpath = Helper::stringizeString( m_serviceProvider, path );

        OutputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( CONST_STRING(m_serviceProvider, user), fullpath );

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls invalid open file %s"
                , m_name.c_str()
                , _fileName.c_str()
                );

            return false;
        }

		const void * data_memory = _data;
		size_t data_size = _size;

		if( Helper::writeStreamArchiveData( m_serviceProvider, stream, m_archivator, GET_MAGIC_NUMBER(MAGIC_ACCOUNT_DATA), GET_MAGIC_VERSION(MAGIC_ACCOUNT_DATA), true, data_memory, data_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls invalid write file %s"
				, m_name.c_str()
				, _fileName.c_str()
				);

			return false;
		}

        return true;
    }
}
