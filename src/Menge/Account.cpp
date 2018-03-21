#include "Account.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/MemoryInterface.h"
#   include "Interface/ArchiveInterface.h"

#include "Consts.h"

#   include "Logger/Logger.h"

#include "Core/IniUtil.h"
#include "Core/String.h"
#include "Core/Stream.h"

namespace Mengine
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
	bool Account::initialize( const ConstString & _id, const FilePath & _folder, uint32_t _projectVersion )
	{
		m_id = _id;
		m_projectVersion = _projectVersion;

		m_folder = _folder;

		PathString settingsPath;
		settingsPath += m_folder;
		settingsPath += "settings.ini";

		m_settingsPath = Helper::stringizeFilePath( settingsPath );

		m_archivator = ARCHIVE_SERVICE()
			->getArchivator( STRINGIZE_STRING_LOCAL( "lz4") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Account::getID() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::setUID( const String & _uid )
	{
		m_uid = _uid;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & Account::getUID() const
	{
		return m_uid;
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
			LOGGER_ERROR("Account::addSetting: account '%s' setting '%s' already exist"
                , m_id.c_str()
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
			LOGGER_ERROR("Account::changeSetting account %s setting '%s' does not exist. Can't change"
                , m_id.c_str()
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
			LOGGER_ERROR("Account::getSetting account '%s' setting '%s' does not exist. Can't get"
                , m_id.c_str()
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
        if( FILE_SERVICE()
			->existFile( STRINGIZE_STRING_LOCAL( "user"), m_settingsPath, nullptr ) == false )
        {
            LOGGER_ERROR("Account::load account '%s' settings not found '%s'"
                , m_id.c_str()
                , m_settingsPath.c_str() 
                );

            return false;
        }

		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, STRINGIZE_STRING_LOCAL( "user" ), m_settingsPath ) == false )
		{
			LOGGER_ERROR("Account::load parsing Account '%s' settings failed '%s'"
                , m_id.c_str()
				, m_settingsPath.c_str() 
				);

			return false;
		}

		String projectVersion_s;
		if( IniUtil::getIniValue( ini, "ACCOUNT", "PROJECT_VERSION", projectVersion_s ) == false )
		{
			LOGGER_ERROR("Account::load account '%s' failed not found project version"
				, m_id.c_str()
				);

			return false;
		}

		uint32_t projectVersion = 0;
		if( Helper::stringToUnsigned( projectVersion_s, projectVersion ) == false )
		{
			LOGGER_ERROR("Account::load account '%s' failed invalid project version '%s'"
				, m_id.c_str()
				, projectVersion_s.c_str()
				);

			return false;
		}

		if( m_projectVersion != projectVersion )
		{
			LOGGER_ERROR("Account::load account '%s' failed invalid project version '%d' need '%d'"
				, m_id.c_str()
				, projectVersion
				, m_projectVersion
				);

			return false;
		}

		String uid;
		if( IniUtil::getIniValue( ini, "ACCOUNT", "UID", uid ) == false )
		{
			LOGGER_ERROR("Account::load account '%s' failed not found uid"
				, m_id.c_str()
				);

			return false;
		}

		m_uid = uid;

		for( TMapSettings::iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		++it )
		{
            const ConstString & key = it->first;
            Setting & st = it->second;

            if( IniUtil::getIniValue( ini, "SETTINGS", key.c_str(), st.value ) == false )
            {
                LOGGER_WARNING("Account::load account '%s' failed get setting '%s'"
                    , m_id.c_str()
                    , key.c_str() 
                    );
            }
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Account::save()
	{
		OutputStreamInterfacePtr file = FILE_SERVICE()
            ->openOutputFile( CONST_STRING( user), m_settingsPath );

		if( file == nullptr )
		{
			LOGGER_ERROR("Account::save can't open file for writing. Account '%s' settings not saved %s"
				, m_id.c_str() 
                , m_settingsPath.c_str()
				);

			return false;
		}

		IniUtil::writeIniSection( file, "[ACCOUNT]" );

		WString projectVersion_s;
		if( Helper::unsignedToWString( m_projectVersion, projectVersion_s ) == false )
		{
			return false;
		}

		WString wuid;
		Helper::utf8ToUnicode( m_uid, wuid );

		IniUtil::writeIniSetting( file, "PROJECT_VERSION", projectVersion_s );
		IniUtil::writeIniSetting( file, "UID", wuid );
        
        IniUtil::writeIniSection( file, "[SETTINGS]" );
        
		for( TMapSettings::const_iterator 
			it = m_settings.begin(), 
			it_end = m_settings.end();
		it != it_end;
		++it )
		{
			const ConstString & key = it->first;
			const Setting & st = it->second;

            IniUtil::writeIniSetting( file, key.c_str(), st.value );
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
		path += _fileName;

		FilePath fullpath = Helper::stringizeFilePath( path );

		InputStreamInterfacePtr stream = 
			FILE_SERVICE()->openInputFile( CONST_STRING( user), fullpath, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR("Account::openReadBinaryFile: account %s invalid open file %s"
				, m_id.c_str()
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
		path += _filepath;

		FilePath fullpath = Helper::stringizeFilePath( path );

		OutputStreamInterfacePtr stream = FILE_SERVICE()
			->openOutputFile( CONST_STRING( user), fullpath );

		if( stream == nullptr )
		{
			LOGGER_ERROR("Account::openWriteBinaryFile: account %s invalid open file %s"
				, m_id.c_str()
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
            LOGGER_ERROR("Account::loadBinaryFile: account %s invalid open file %s"
                , m_id.c_str()
                , _filepath.c_str()
                );

            return nullptr;
        }
		
		MemoryInterfacePtr binaryBuffer;		
		if( Helper::loadStreamArchiveData( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), binaryBuffer, __FILE__, __LINE__ ) == false )
		{
			LOGGER_ERROR("Account::loadBinaryFile: account %s invalid load stream archive %s"
				, m_id.c_str()
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
			LOGGER_ERROR("Account::writeBinaryFile: account %s write empty file %s"
				, m_id.c_str()
				, _filepath.c_str()
				);

			return false;
		}

        OutputStreamInterfacePtr stream = this->openWriteBinaryFile( _filepath );

        if( stream == nullptr )
        {
            LOGGER_ERROR("Account::writeBinaryFile: account %s invalid open file %s"
                , m_id.c_str()
                , _filepath.c_str()
                );

            return false;
        }

		const void * data_memory = _data;
		size_t data_size = _size;

		if( Helper::writeStreamArchiveData( stream, m_archivator, GET_MAGIC_NUMBER(MAGIC_ACCOUNT_DATA), GET_MAGIC_VERSION(MAGIC_ACCOUNT_DATA), true, data_memory, data_size, EAC_NORMAL ) == false )
		{
			LOGGER_ERROR("Account::writeBinaryFile: account %s invalid write file %s"
				, m_id.c_str()
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
		path += _fileName;

		FilePath fullpath = Helper::stringizeFilePath( path );

		bool exist = FILE_SERVICE()
			->existFile( CONST_STRING( user), fullpath, nullptr );

		return exist;
	}
}
