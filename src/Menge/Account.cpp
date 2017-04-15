#	include "Account.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/MemoryInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Consts.h"

#   include "Logger/Logger.h"

#	include "Core/IniUtil.h"
#	include "Core/String.h"
#	include "Core/Stream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Account::Account()
		: m_projectVersion(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Account::~Account()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Account::initialize( const WString & _name, const FilePath & _folder, uint32_t _projectVersion )
	{
		m_name = _name;
		m_projectVersion = _projectVersion;

		m_folder = _folder;

		PathString settingsPath;
		settingsPath += m_folder;
        settingsPath += '_';
		settingsPath += "settings.ini";

		m_settingsPath = Helper::stringizeFilePath( m_serviceProvider, settingsPath );

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
    const FilePath & Account::getFolder() const
    {
        return m_folder;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Account::addSetting( const ConstString & _setting, const WString& _defaultValue, const pybind::object & _applyFunc )
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

        Setting st;
        st.value = _defaultValue;
        st.cb = _applyFunc;
        
        m_settings.insert( std::make_pair(_setting, st) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Account::changeSetting( const ConstString & _setting, const WString& _value )
	{
		TMapSettings::iterator it_found = m_settings.find( _setting );

		if( it_found == m_settings.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::changeSetting account %ls setting '%s' does not exist. Can't change"
                , m_name.c_str()
				, _setting.c_str()
				);

			return false;
		}

		Setting & st = it_found->second;

		st.value = _value;

		if( st.cb.is_none() == false )
		{
			st.cb.call( _value );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	WString Account::getSetting( const ConstString & _setting ) const
	{
		TMapSettings::const_iterator it_found = m_settings.find( _setting );

		if( it_found == m_settings.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::getSetting account '%ls' setting '%s' does not exist. Can't get"
                , m_name.c_str()
				, _setting.c_str()
				);

			return WString();
		}

		const Setting & st = it_found->second;

		return st.value;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Account::hasSetting( const ConstString & _setting ) const
    {
		TMapSettings::const_iterator it_found = m_settings.find( _setting );

		if( it_found == m_settings.end() )
		{
			return false;
		}

        return true;
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
            const ConstString & key = it->first;
            Setting & st = it->second;

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
           
			if( st.cb.is_none() == true )
			{
				continue;
			}

			st.cb.call( st.value );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr Account::openReadBinaryFile( const FilePath & _fileName )
	{
		PathString path;

		path += m_folder;
		path += '_';
		path += _fileName;

		FilePath fullpath = Helper::stringizeFilePath( m_serviceProvider, path );

		InputStreamInterfacePtr stream = 
			FILE_SERVICE(m_serviceProvider)->openInputFile( CONST_STRING(m_serviceProvider, user), fullpath, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::openReadBinaryFile: account %ls invalid open file %s"
				, m_name.c_str()
				, fullpath.c_str()
				);

			return nullptr;
		}

		return stream;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterfacePtr Account::openWriteBinaryFile( const FilePath & _filepath )
	{
		PathString path;

		path += m_folder;
		path += '_';
		path += _filepath;

		FilePath fullpath = Helper::stringizeFilePath( m_serviceProvider, path );

		OutputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openOutputFile( CONST_STRING(m_serviceProvider, user), fullpath );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::openWriteBinaryFile: account %ls invalid open file %s"
				, m_name.c_str()
				, _filepath.c_str()
				);

			return nullptr;
		}

		return stream;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInterfacePtr Account::loadBinaryFile( const FilePath & _filepath )
    {        
        InputStreamInterfacePtr stream = this->openReadBinaryFile( _filepath );           

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::loadBinaryFile: account %ls invalid open file %s"
                , m_name.c_str()
                , _filepath.c_str()
                );

            return nullptr;
        }
		
		MemoryInterfacePtr binaryBuffer;		
		if( Helper::loadStreamArchiveData( m_serviceProvider, stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), binaryBuffer, __FILE__, __LINE__ ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::loadBinaryFile: account %ls invalid load stream archive %s"
				, m_name.c_str()
				, _filepath.c_str()
				);

			return nullptr;
		}

		return binaryBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::writeBinaryFile( const FilePath & _filepath, const void * _data, size_t _size )
    {
		if( _data == nullptr || _size == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls write empty file %s"
				, m_name.c_str()
				, _filepath.c_str()
				);

			return false;
		}

        OutputStreamInterfacePtr stream = this->openWriteBinaryFile( _filepath );

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls invalid open file %s"
                , m_name.c_str()
                , _filepath.c_str()
                );

            return false;
        }

		const void * data_memory = _data;
		size_t data_size = _size;

		if( Helper::writeStreamArchiveData( m_serviceProvider, stream, m_archivator, GET_MAGIC_NUMBER(MAGIC_ACCOUNT_DATA), GET_MAGIC_VERSION(MAGIC_ACCOUNT_DATA), true, data_memory, data_size, EAC_NORMAL ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Account::writeBinaryFile: account %ls invalid write file %s"
				, m_name.c_str()
				, _filepath.c_str()
				);

			return false;
		}

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Account::hasBinaryFile( const FilePath & _fileName ) const
	{
		PathString path;

		path += m_folder;
        path += '_';
		path += _fileName;

		FilePath fullpath = Helper::stringizeFilePath( m_serviceProvider, path );

		bool exist = FILE_SERVICE(m_serviceProvider)
			->existFile( CONST_STRING(m_serviceProvider, user), fullpath, nullptr );

		return exist;
	}
}
