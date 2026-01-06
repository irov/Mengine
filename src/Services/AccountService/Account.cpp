#include "Account.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/StreamHelper.h"
#include "Kernel/IniHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/PathString.h"
#include "Kernel/UID.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/ConfigHelper.h"

#include "Config/StdString.h"

#include "stdex/memorycopy.h"

#ifndef MENGINE_ACCOUNT_SETTINGS_JSON_PATH
#define MENGINE_ACCOUNT_SETTINGS_JSON_PATH "settings.json"
#endif

#ifndef MENGINE_ACCOUNT_SETTINGS_INI_PATH
#define MENGINE_ACCOUNT_SETTINGS_INI_PATH "settings.ini"
#endif

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
        m_accountId = _id;
        m_archivator = _archivator;
        m_fileGroup = _fileGroup;
        m_folderName = _folderPath;
        m_projectVersion = _projectVersion;

        PathString settingsJSONPath;
        settingsJSONPath += m_folderName;
        settingsJSONPath += MENGINE_ACCOUNT_SETTINGS_JSON_PATH;

        FilePath settingsJSONPath_f = Helper::stringizeFilePath( settingsJSONPath );

        ContentInterfacePtr settingsJSONContent = Helper::makeFileContent( m_fileGroup, settingsJSONPath_f, MENGINE_DOCUMENT_FACTORABLE );

        m_settingsJSONContent = settingsJSONContent;

        PathString settingsINIPath;
        settingsINIPath += m_folderName;
        settingsINIPath += MENGINE_ACCOUNT_SETTINGS_INI_PATH;

        FilePath settingsINIPath_f = Helper::stringizeFilePath( settingsINIPath );

        ContentInterfacePtr settingsINIContent = Helper::makeFileContent( m_fileGroup, settingsINIPath_f, MENGINE_DOCUMENT_FACTORABLE );

        m_settingsINIContent = settingsINIContent;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Account::finalize()
    {
        m_archivator = nullptr;
        m_fileGroup = nullptr;

        m_settingsJSONContent = nullptr;
        m_settingsINIContent = nullptr;        

        m_settings.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Account::getAccountId() const
    {
        return m_accountId;
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
    const FilePath & Account::getFolderName() const
    {
        return m_folderName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::addSetting( const ConstString & _setting, const Char * _defaultValue, const AccountSettingProviderInterfacePtr & _provider )
    {
        MapSettings::iterator it = m_settings.find( _setting );

        if( it != m_settings.end() )
        {
            LOGGER_ERROR( "account '%s' setting '%s' already exist"
                , m_accountId.c_str()
                , _setting.c_str()
            );

            return false;
        }

        Setting st;
        st.value.assign( _defaultValue );
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
                , m_accountId.c_str()
                , _setting.c_str()
            );

            return false;
        }

        Setting & st = it_found->second;

        st.value.assign( _value );

        if( this->save() == false )
        {
            LOGGER_ERROR( "account '%s' setting '%s' save failed"
                , m_accountId.c_str()
                , _setting.c_str()
            );
        }

        if( st.provider != nullptr )
        {
            st.provider->onChangeSetting( _value );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::getSetting( const ConstString & _setting, const Char ** const _value ) const
    {
        MapSettings::const_iterator it_found = m_settings.find( _setting );

        if( it_found == m_settings.end() )
        {
            LOGGER_ERROR( "account '%s' setting '%s' does not exist. Can't get"
                , m_accountId.c_str()
                , _setting.c_str()
            );

            return false;
        }

        const Setting & st = it_found->second;

        *_value = st.value.c_str();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::hasSetting( const ConstString & _setting, const Char ** const _value ) const
    {
        MapSettings::const_iterator it_found = m_settings.find( _setting );

        if( it_found == m_settings.end() )
        {
            return false;
        }

        if( _value != nullptr )
        {
            const Setting & st = it_found->second;

            *_value = st.value.c_str();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::load()
    {
        ConfigInterfacePtr config = this->getLoadSettingConfig_();

        if( config == nullptr )
        {
            return false;
        }

        uint32_t projectVersion;
        if( config->hasValueInteger( "ACCOUNT", "PROJECT_VERSION", 0U, &projectVersion ) == false )
        {
            LOGGER_ERROR( "account '%s' failed not found project version"
                , m_accountId.c_str()
            );

            return false;
        }

        if( m_projectVersion != projectVersion )
        {
            LOGGER_ERROR( "account '%s' failed invalid project version '%u' need '%u'"
                , m_accountId.c_str()
                , projectVersion
                , m_projectVersion
            );

            return false;
        }

        String uid;
        if( config->hasValue( "ACCOUNT", "UID", String(), &uid ) == false )
        {
            LOGGER_ERROR( "account '%s' failed not found uid"
                , m_accountId.c_str()
            );

            return false;
        }

        MENGINE_ASSERTION_FATAL( uid.size() == 20, "account '%s' invalid uid '%s'"
            , m_accountId.c_str()
            , uid.c_str()
        );

        stdex::memorycopy( m_uid.data, 0, uid.c_str(), 20 );

        LOGGER_INFO( "account", "load account '%s' projectVersion [%u] uid '%.20s' settings"
            , m_accountId.c_str()
            , projectVersion
            , m_uid.data
        );

        for( auto && [key, st] : m_settings )
        {
            String value;
            if( config->hasValue( "SETTINGS", key.c_str(), String(), &value ) == false )
            {
                continue;
            }

            LOGGER_INFO( "account", "load setting '%s' value '%s'"
                , key.c_str()
                , value.c_str()
            );

            st.value.assign( value );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigInterfacePtr Account::getLoadSettingConfig_() const
    {
        if( m_settingsJSONContent->exist( true ) == true )
        {
            ConfigInterfacePtr config = Helper::loadConfig( m_settingsJSONContent, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            if( config == nullptr )
            {
                LOGGER_ERROR( "get account '%s' settings failed load '%s'"
                    , m_accountId.c_str()
                    , Helper::getContentFullPath( m_settingsJSONContent ).c_str()
                );

                return nullptr;
            }

            return config;
        }

        if( m_settingsINIContent->exist( true ) == true )
        {
            ConfigInterfacePtr config = Helper::loadConfig( m_settingsINIContent, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            if( config == nullptr )
            {
                LOGGER_ERROR( "get account '%s' settings failed load '%s'"
                    , m_accountId.c_str()
                    , Helper::getContentFullPath( m_settingsINIContent ).c_str()
                );

                return nullptr;
            }

            return config;
        }

        LOGGER_ERROR( "account '%s' settings not found any config '%s' or '%s'"
            , m_accountId.c_str()
            , Helper::getContentFullPath( m_settingsJSONContent ).c_str()
            , Helper::getContentFullPath( m_settingsINIContent ).c_str()
        );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::save()
    {
        jpp::object j_root = jpp::make_object();

        jpp::object j_account = jpp::make_object();

        j_account.set( "PROJECT_VERSION", m_projectVersion );
        j_account.set( "UID", jpp::make_stringn( m_uid.data, AccountUID::size_data ) );

        LOGGER_INFO( "account", "save account '%s' projectVersion [%u] uid '%.20s' settings"
            , m_accountId.c_str()
            , m_projectVersion
            , m_uid.data
        );

        jpp::object j_settings = jpp::make_object();

        for( auto && [key, st] : m_settings )
        {
            const Char * key_str = key.c_str();
            const Char * value_str = st.value.c_str();

            LOGGER_INFO( "account", "save setting '%s' value '%s'"
                , key_str
                , value_str
            );

            j_settings.set( key_str, value_str );
        }

        j_root.set( "ACCOUNT", j_account );
        j_root.set( "SETTINGS", j_settings );

        const FilePath & settingsJSONPath = m_settingsJSONContent->getFilePath();

        if( Helper::writeJSONFile( j_root, m_fileGroup, settingsJSONPath, true, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "account '%s' invalid write json to file '%s'"
                , m_accountId.c_str()
                , Helper::getContentFullPath( m_settingsJSONContent ).c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Account::apply()
    {
        for( auto && [key, st] : m_settings )
        {
            if( st.provider == nullptr )
            {
                continue;
            }

            const Char * value_str = st.value.c_str();

            st.provider->onChangeSetting( value_str );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Account::openReadBinaryFile( const FilePath & _filePath )
    {
        PathString path;
        path += m_folderName;
        path += _filePath;

        FilePath fullFilePath = Helper::stringizeFilePath( path );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( m_fileGroup, fullFilePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_accountId.c_str()
                , fullFilePath.c_str()
            );

            return nullptr;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    void Account::closeReadBinaryFile( const InputStreamInterfacePtr & _stream )
    {
        m_fileGroup->closeInputFile( _stream );
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Account::openWriteBinaryFile( const FilePath & _filePath )
    {
        PathString path;
        path += m_folderName;
        path += _filePath;

        FilePath fullFilePath = Helper::stringizeFilePath( path );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, fullFilePath, true, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_accountId.c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::closeWriteBinaryFile( const OutputStreamInterfacePtr & _stream )
    {
        bool result = m_fileGroup->closeOutputFile( _stream );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr Account::loadBinaryFile( const FilePath & _filePath )
    {
        InputStreamInterfacePtr stream = this->openReadBinaryFile( _filePath );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_accountId.c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        MemoryInterfacePtr binaryBuffer = Helper::readStreamArchiveMagic( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), MENGINE_DOCUMENT_FACTORABLE );

        this->closeReadBinaryFile( stream );

        if( binaryBuffer == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid load stream archive '%s'"
                , m_accountId.c_str()
                , _filePath.c_str()
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
            LOGGER_ERROR( "account '%s' write empty file '%s'"
                , m_accountId.c_str()
                , _filepath.c_str()
            );

            return false;
        }

        OutputStreamInterfacePtr stream = this->openWriteBinaryFile( _filepath );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_accountId.c_str()
                , _filepath.c_str()
            );

            return false;
        }

        const void * data_memory = _data;
        size_t data_size = _size;

        bool successful = Helper::writeStreamArchiveMagic( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), true, data_memory, data_size, EAC_NORMAL );

        this->closeWriteBinaryFile( stream );

        if( successful == false )
        {
            LOGGER_ERROR( "account '%s' invalid write file '%s'"
                , m_accountId.c_str()
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
        path += m_folderName;
        path += _filePath;

        FilePath fullpath = Helper::stringizeFilePath( path );

        bool exist = m_fileGroup->existFile( fullpath, true );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
}