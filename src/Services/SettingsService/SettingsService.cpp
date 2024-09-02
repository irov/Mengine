#include "SettingsService.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SettingsService, Mengine::SettingsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SettingsService::SettingsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SettingsService::~SettingsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SettingsService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SettingsService::_finalizeService()
    {
        for( const HashtableSettings::value_type & value : m_settings )
        {
            const SettingInterfacePtr & setting = value.element;

            setting->finalize();
        }

        m_settings.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SettingsService::loadSetting( const ConstString & _name, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        const FilePath & filePath = _content->getFilePath();

        ConstString filePathExt = Helper::getFilePathExt( filePath );

        PrototypeGeneratorInterfacePtr generator;
        if( PROTOTYPE_SERVICE()
            ->hasGenerator( STRINGIZE_STRING_LOCAL( "Setting" ), filePathExt, &generator ) == false )
        {
            LOGGER_ERROR( "setting '%s' file '%s' not found ext '%s' generator"
                , _name.c_str()
                , Helper::getContentFullPath( _content )
                , filePathExt.c_str()
            );

            return false;
        }

        SettingInterfacePtr setting = generator->generate( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( setting );

        if( setting->initialize( _content, _doc ) == false )
        {
            return false;
        }

        m_settings.emplace( _name, setting );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SettingsService::unloadSetting( const ConstString & _name )
    {
        SettingInterfacePtr setting = m_settings.erase( _name );

        if( setting == nullptr )
        {
            return false;
        }

        setting->finalize();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SettingsService::loadSettings( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "settings", "load settings... '%s'"
            , Helper::getContentFullPath( _content )
        );

        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _content, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid load settings '%s'"
            , Helper::getContentFullPath( _content )
        );

        VectorString settingSettings;
        config->getValues( "GAME_SETTINGS", "Setting", &settingSettings );

        for( const String & setting : settingSettings )
        {
            LOGGER_INFO( "settings", "setting load '%s'"
                , setting.c_str()
            );

            if( config->hasSection( setting.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid load '%s' settings no found section for '%s'"
                    , Helper::getContentFullPath( _content )
                    , setting.c_str()
                );

                return false;
            }

            ConstString name;
            FilePath filePath;
            config->hasValue( setting.c_str(), "Name", ConstString::none(), &name );
            config->hasValue( setting.c_str(), "Path", FilePath::none(), &filePath );

            const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();

            ContentInterfacePtr settingContent = Helper::makeFileContent( fileGroup, filePath, _doc );

            if( this->loadSetting( name, settingContent, _doc ) == false )
            {
                LOGGER_ERROR( "invalid load setting '%s' file '%s'"
                    , name.c_str()
                    , filePath.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const SettingInterfacePtr & SettingsService::getSetting( const ConstString & _name ) const
    {
        const SettingInterfacePtr & setting = m_settings.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( setting );

        return setting;
    }
    //////////////////////////////////////////////////////////////////////////
    SettingInterfacePtr SettingsService::removeSetting( const ConstString & _name )
    {
        SettingInterfacePtr setting_remove = m_settings.erase( _name );

        return setting_remove;
    }
    //////////////////////////////////////////////////////////////////////////
    void SettingsService::foreachSettings( const LambdaSettings & _lambda ) const
    {
        for( const HashtableSettings::value_type & value : m_settings )
        {
            _lambda( value.key, value.element );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
