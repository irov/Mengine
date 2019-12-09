#include "Account.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Stream.h"
#include "Kernel/IniHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/StringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Account::Account()
        : m_projectVersion( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Account::~Account()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::initialize( const ConstString & _id, const ArchivatorInterfacePtr & _archivator, const FileGroupInterfacePtr & _fileGroup, const FilePath & _folderPath, uint32_t _projectVersion )
    {
        m_id = _id;
        m_projectVersion = _projectVersion;

        m_folderPath = _folderPath;

        PathString settingsPath;
        settingsPath += m_folderPath;
        settingsPath += "settings.ini";

        m_settingsPath = Helper::stringizeFilePath( settingsPath );

        m_archivator = _archivator;

        m_fileGroup = _fileGroup;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Account::finalize()
    {
        m_archivator = nullptr;
        m_fileGroup = nullptr;

        m_settings.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Account::getID() const
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void Account::setUID( const AccountUID & _uid )
    {
        m_uid = _uid;
    }
    //////////////////////////////////////////////////////////////////////////
    const AccountUID & Account::getUID() const
    {
        return m_uid;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Account::getFolder() const
    {
        return m_folderPath;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::addSetting( const ConstString & _setting, const Char * _defaultValue, const AccountSettingProviderInterfacePtr & _provider )
    {
        MapSettings::iterator it = m_settings.find( _setting );

        if( it != m_settings.end() )
        {
            LOGGER_ERROR( "account '%s' setting '%s' already exist"
                , m_id.c_str()
                , _setting.c_str()
            );

            return false;
        }

        Setting st;
        ::strcpy( st.value, _defaultValue );
        st.provider = _provider;

        m_settings.emplace( _setting, st );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::changeSetting( const ConstString & _setting, const Char * _value )
    {
        MapSettings::iterator it_found = m_settings.find( _setting );

        if( it_found == m_settings.end() )
        {
            LOGGER_ERROR( "account '%s' setting '%s' does not exist. Can't change"
                , m_id.c_str()
                , _setting.c_str()
            );

            return false;
        }

        Setting & st = it_found->second;

        ::strcpy( st.value, _value );

        if( st.provider != nullptr )
        {
            st.provider->onChangeSetting( _value );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::getSetting( const ConstString & _setting, Char * _value ) const
    {
        MapSettings::const_iterator it_found = m_settings.find( _setting );

        if( it_found == m_settings.end() )
        {
            LOGGER_ERROR( "account '%s' setting '%s' does not exist. Can't get"
                , m_id.c_str()
                , _setting.c_str()
            );

            return false;
        }

        const Setting & st = it_found->second;

        ::strcpy( _value, st.value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::hasSetting( const ConstString & _setting, Char * _value ) const
    {
        MapSettings::const_iterator it_found = m_settings.find( _setting );

        if( it_found == m_settings.end() )
        {
            return false;
        }

        if( _value != nullptr )
        {
            const Setting & st = it_found->second;

            ::strcpy( _value, st.value );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::load()
    {
        if( m_fileGroup->existFile( m_settingsPath, true ) == false )
        {
            LOGGER_ERROR( "account '%s' settings not found '%s'"
                , m_id.c_str()
                , m_settingsPath.c_str()
            );

            return false;
        }

        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( m_fileGroup, m_settingsPath, MENGINE_DOCUMENT_FUNCTION );

        if( config == nullptr )
        {
            LOGGER_ERROR( "parsing Account '%s' settings failed '%s'"
                , m_id.c_str()
                , m_settingsPath.c_str()
            );

            return false;
        }

        const Char * projectVersion_s;
        if( config->hasValue( "ACCOUNT", "PROJECT_VERSION", &projectVersion_s ) == false )
        {
            LOGGER_ERROR( "account '%s' failed not found project version"
                , m_id.c_str()
            );

            return false;
        }

        uint32_t projectVersion = 0;
        if( Helper::stringToUnsigned( projectVersion_s, &projectVersion ) == false )
        {
            LOGGER_ERROR( "account '%s' failed invalid project version '%s'"
                , m_id.c_str()
                , projectVersion_s
            );

            return false;
        }

        if( m_projectVersion != projectVersion )
        {
            LOGGER_ERROR( "account '%s' failed invalid project version '%d' need '%d'"
                , m_id.c_str()
                , projectVersion
                , m_projectVersion
            );

            return false;
        }

        const Char * uid;
        if( config->hasValue( "ACCOUNT", "UID", &uid ) == false )
        {
            LOGGER_ERROR( "account '%s' failed not found uid"
                , m_id.c_str()
            );

            return false;
        }

        ::strncpy( m_uid.data, uid, AccountUID::size_data );

        for( MapSettings::iterator
            it = m_settings.begin(),
            it_end = m_settings.end();
            it != it_end;
            ++it )
        {
            const ConstString & key = it->first;
            Setting & st = it->second;

            const Char * value;
            if( config->hasValue( "SETTINGS", key.c_str(), &value ) == false )
            {
                LOGGER_MESSAGE( "account '%s' failed get setting '%s'"
                    , m_id.c_str()
                    , key.c_str()
                );

                continue;
            }

            ::strcpy( st.value, value );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::save()
    {
        OutputStreamInterfacePtr file = Helper::openOutputStreamFile( m_fileGroup, m_settingsPath, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( file, false, "can't open file for writing. Account '%s' settings not saved '%s'"
            , m_id.c_str()
            , m_settingsPath.c_str()
        );

        Helper::writeIniSection( file, "[ACCOUNT]" );

        String projectVersion_s;
        if( Helper::unsignedToString( m_projectVersion, &projectVersion_s ) == false )
        {
            return false;
        }

        Helper::writeIniSetting( file, "PROJECT_VERSION", projectVersion_s );
        Helper::writeIniSetting( file, "UID", m_uid.data, AccountUID::size_data );

        Helper::writeIniSection( file, "[SETTINGS]" );

        for( MapSettings::const_iterator
            it = m_settings.begin(),
            it_end = m_settings.end();
            it != it_end;
            ++it )
        {
            const ConstString & key = it->first;
            const Setting & st = it->second;

            Helper::writeIniSetting( file, key.c_str(), st.value );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Account::apply()
    {
        for( MapSettings::iterator
            it = m_settings.begin(),
            it_end = m_settings.end();
            it != it_end;
            ++it )
        {
            Setting & st = it->second;

            if( st.provider == nullptr )
            {
                continue;
            }

            st.provider->onChangeSetting( st.value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Account::openReadBinaryFile( const FilePath & _filePath )
    {
        PathString path;

        path += m_folderPath;
        path += _filePath;

        FilePath fullpath = Helper::stringizeFilePath( path );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( m_fileGroup, fullpath, false, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr, "account '%s' invalid open file '%s'"
            , m_id.c_str()
            , fullpath.c_str()
        );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Account::openWriteBinaryFile( const FilePath & _filepath )
    {
        PathString path;

        path += m_folderPath;
        path += _filepath;

        FilePath fullpath = Helper::stringizeFilePath( path );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, fullpath, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr, "account '%s' invalid open file '%s'"
            , m_id.c_str()
            , _filepath.c_str()
        );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr Account::loadBinaryFile( const FilePath & _filepath )
    {
        InputStreamInterfacePtr stream = this->openReadBinaryFile( _filepath );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr, "account '%s' invalid open file '%s'"
            , m_id.c_str()
            , _filepath.c_str()
        );

        MemoryInterfacePtr binaryBuffer = Helper::loadStreamArchiveData( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( binaryBuffer, nullptr, "account '%s' invalid load stream archive '%s'"
            , m_id.c_str()
            , _filepath.c_str()
        );

        return binaryBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::writeBinaryFile( const FilePath & _filepath, const void * _data, size_t _size )
    {
        if( _data == nullptr || _size == 0 )
        {
            LOGGER_ERROR( "account '%s' write empty file '%s'"
                , m_id.c_str()
                , _filepath.c_str()
            );

            return false;
        }

        OutputStreamInterfacePtr stream = this->openWriteBinaryFile( _filepath );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, false, "account '%s' invalid open file '%s'"
            , m_id.c_str()
            , _filepath.c_str()
        );

        const void * data_memory = _data;
        size_t data_size = _size;

        if( Helper::writeStreamArchiveData( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), true, data_memory, data_size, EAC_NORMAL ) == false )
        {
            LOGGER_ERROR( "account '%s' invalid write file '%s'"
                , m_id.c_str()
                , _filepath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::hasBinaryFile( const FilePath & _filePath ) const
    {
        PathString path;
        path += m_folderPath;
        path += _filePath;

        FilePath fullpath = Helper::stringizeFilePath( path );

        bool exist = m_fileGroup->existFile( fullpath, true );

        return exist;
    }
}
