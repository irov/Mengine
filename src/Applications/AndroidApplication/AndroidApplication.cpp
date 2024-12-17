#include "AndroidApplication.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PreferencesSystemInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidLogger.h"

#include "Mengine/MenginePlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"

#include "Config/Algorithm.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidApplication::AndroidApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidApplication::~AndroidApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidApplication::initializeOptionsService_( int32_t _argc, Char ** const _argv )
    {
        ArgumentsInterfacePtr arguments = Helper::makeFactorableUnique<StringArguments>( MENGINE_DOCUMENT_FUNCTION );

#if !defined(MENGINE_BUILD_PUBLISH)
        Char MengineApplePersistentArguments[1024 + 1] = {'\0'};
        if( PREFERENCES_SYSTEM()
            ->getPreferenceString( "persistent_arguments", MengineApplePersistentArguments, 1024, nullptr ) == true )
        {
            arguments->addArguments( MengineApplePersistentArguments );
        }
#endif

        for( int32_t i = 1; i < _argc; ++i )
        {
            const Char * arg = _argv[i];

            arguments->addArgument( arg );
        }

        if( OPTIONS_SERVICE()
            ->setArguments( arguments ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidApplication::initializeLoggerService_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

        LoggerInterfacePtr loggerAndroid = Helper::makeFactorableUnique<AndroidLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerAndroid->setWriteHistory(true );

        if(LOGGER_SERVICE()
            ->registerLogger(loggerAndroid ) == true )
        {
            m_loggerAndroid = loggerAndroid;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidApplication::finalizeLoggerService_()
    {
        if(m_loggerAndroid != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger(m_loggerAndroid );

            m_loggerAndroid = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidApplication::bootstrap( const Mengine::Char * _nativeLibraryDir, int32_t _argc, Char ** const _argv )
    {
        MENGINE_UNUSED( _nativeLibraryDir );

        ServiceProviderInterface * serviceProvider = API_MengineCreate();

        if( serviceProvider == nullptr )
        {
            return false;
        }

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_PROVIDER_GET()
            ->waitService( "AndroidApplication", SERVICE_ID( OptionsServiceInterface ), [this, _argc, _argv]()
        {
            if( this->initializeOptionsService_( _argc, _argv ) == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_WAIT( AndroidApplication, LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_LEAVE( AndroidApplication, LoggerServiceInterface, [this]()
        {
            this->finalizeLoggerService_();
        } );

        if( API_MengineBootstrap() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidApplication::initialize()
    {
        if( API_MengineRun() == false )
        {
            return false;
        }

        LOGGER_INFO( "application", "creating render window..." );

        const Char * projectTitle = nullptr;
        size_t projectTitleLen = 0;

        ConstString APPLICATION_TITLE = CONFIG_VALUE( "Window", "ApplicationTitleTextId", STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ) );

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( APPLICATION_TITLE, &entry ) == false )
        {
            LOGGER_MESSAGE( "application not setup title '%s'"
                , APPLICATION_TITLE.c_str()
            );
        }
        else
        {
            projectTitle = entry->getValue( &projectTitleLen );
        }

        PLATFORM_SERVICE()
            ->setProjectTitle( projectTitle );

        PLATFORM_SERVICE()
            ->updatePlatform();

        if( PLATFORM_SERVICE()
            ->createWindow( Resolution( 0, 0 ), true ) == false )
        {
            LOGGER_FATAL( "invalid create window" );

            return false;
        }

        if( APPLICATION_SERVICE()
            ->createRenderWindow() == false )
        {
            LOGGER_FATAL( "invalid create render window" );

            return false;
        }

        GAME_SERVICE()
            ->run();

        PLATFORM_SERVICE()
            ->runPlatform();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidApplication::loop()
    {
        PLATFORM_SERVICE()
            ->loopPlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidApplication::finalize()
    {
        if( SERVICE_PROVIDER_EXIST() == false )
        {
            return;
        }
        
        if( SERVICE_IS_INITIALIZE( Mengine::PlatformServiceInterface ) == true )
        {
            PLATFORM_SERVICE()
                ->stopPlatform();
        }

        if( SERVICE_IS_INITIALIZE( Mengine::BootstrapperInterface ) == true )
        {
            BOOTSTRAPPER_SERVICE()
                ->stop();
        }

        API_MengineFinalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
