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
#include "Kernel/Stringalized.h"
#include "Kernel/PathString.h"
#include "Kernel/UID.h"

#include "Config/StdString.h"

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

        Helper::makeUID( 20, m_uid.data );

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
        MENGINE_STRCPY( st.value, _defaultValue );
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

        MENGINE_STRCPY( st.value, _value );

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
                , m_id.c_str()
                , _setting.c_str()
            );

            return false;
        }

        const Setting & st = it_found->second;

        *_value = st.value;

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

            *_value = st.value;
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
            ->loadConfig( m_fileGroup, m_settingsPath, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

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
        if( Helper::stringalized( projectVersion_s, &projectVersion ) == false )
        {
            LOGGER_ERROR( "account '%s' failed invalid project version '%s'"
                , m_id.c_str()
                , projectVersion_s
            );

            return false;
        }

        if( m_projectVersion != projectVersion )
        {
            LOGGER_ERROR( "account '%s' failed invalid project version '%u' need '%u'"
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

        MENGINE_STRNCPY( m_uid.data, uid, AccountUID::size_data );

        for( auto && [key, st] : m_settings )
        {
            const Char * value;
            if( config->hasValue( "SETTINGS", key.c_str(), &value ) == false )
            {
                LOGGER_MESSAGE( "account '%s' failed get setting '%s'"
                    , m_id.c_str()
                    , key.c_str()
                );

                continue;
            }

            MENGINE_STRCPY( st.value, value );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::save()
    {
        OutputStreamInterfacePtr file = Helper::openOutputStreamFile( m_fileGroup, m_settingsPath, true, MENGINE_DOCUMENT_FACTORABLE );

        if( file == nullptr )
        {
            LOGGER_ERROR( "can't open file for writing. Account '%s' settings not saved '%s'"
                , m_id.c_str()
                , m_settingsPath.c_str()
            );

            return false;
        }

        Helper::writeIniSection( file, "[ACCOUNT]" );

        Helper::writeIniSetting( file, "PROJECT_VERSION", m_projectVersion );
        Helper::writeIniSetting( file, "UID", m_uid.data, AccountUID::size_data );

        Helper::writeIniSection( file, "[SETTINGS]" );

        for( auto && [key, st] : m_settings )
        {
            Helper::writeIniSetting( file, key.c_str(), st.value );
        }

        if( m_fileGroup->closeOutputFile( file ) == false )
        {
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

            st.provider->onChangeSetting( st.value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Account::openReadBinaryFile( const FilePath & _filePath )
    {
        PathString path;
        path += m_folderPath;
        path += _filePath;

        FilePath fullFilePath = Helper::stringizeFilePath( path );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( m_fileGroup, fullFilePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_id.c_str()
                , fullFilePath.c_str()
            );

            return nullptr;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Account::openWriteBinaryFile( const FilePath & _filePath )
    {
        PathString path;
        path += m_folderPath;
        path += _filePath;

        FilePath fullFilePath = Helper::stringizeFilePath( path );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, fullFilePath, true, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_id.c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::closeReadBinaryFile( const InputStreamInterfacePtr & _stream )
    {
        bool successful = m_fileGroup->closeInputFile( _stream );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::openWriteBinaryFile( const OutputStreamInterfacePtr & _stream )
    {
        bool successful = m_fileGroup->closeOutputFile( _stream );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr Account::loadBinaryFile( const FilePath & _filePath )
    {
        InputStreamInterfacePtr stream = this->openReadBinaryFile( _filePath );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_id.c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        MemoryInterfacePtr binaryBuffer = Helper::loadStreamArchiveData( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), MENGINE_DOCUMENT_FACTORABLE );

        if( binaryBuffer == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid load stream archive '%s'"
                , m_id.c_str()
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
                , m_id.c_str()
                , _filepath.c_str()
            );

            return false;
        }

        OutputStreamInterfacePtr stream = this->openWriteBinaryFile( _filepath );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_id.c_str()
                , _filepath.c_str()
            );

            return false;
        }

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

        if( m_fileGroup->closeOutputFile( stream ) == false )
        {
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
    //////////////////////////////////////////////////////////////////////////
}
