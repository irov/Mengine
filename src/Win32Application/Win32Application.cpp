#include "Win32Application.h"

#include "Interface/PlatformInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Kernel/Logger.h"

#include <clocale>
#include <memory>
#include <cerrno>
#include <ctime>
#include <algorithm>
#include <functional>
#include <iomanip>

#include "resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef ServiceProviderInterface * (*FMengineInitialize)(void);
    typedef void (*FMengineFinalize)(void);
    //////////////////////////////////////////////////////////////////////////
    Win32Application::Win32Application()
        : m_hInstance( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32Application::~Win32Application()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initialize()
    {
        HINSTANCE hInstance = ::LoadLibrary( L"Mengine.dll" );

        if( hInstance == NULL )
        {
            return false;
        }

        FARPROC proc = ::GetProcAddress( hInstance, "initializeMengine" );

        if( proc == nullptr )
        {
            ::FreeLibrary( hInstance );

            return false;
        }

        FMengineInitialize dlmengineInitialize = (FMengineInitialize)proc;

        ServiceProviderInterface * serviceProvider = dlmengineInitialize();

        if( serviceProvider == nullptr )
        {
            ::FreeLibrary( hInstance );

            return false;
        }

        m_hInstance = hInstance;

        SERVICE_PROVIDER_SETUP( serviceProvider );

        if( BOOTSTRAPPER_SERVICE()
            ->run() == false )
        {
            LOGGER_CRITICAL( "invalid run bootstrap" );

            return false;
        }

        LOGGER_MESSAGE( "Creating Render Window..." );

        const Char * projectTitle = nullptr;
        size_t projectTitleLen = 0;

        ConstString APPLICATION_TITLE = STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" );

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( APPLICATION_TITLE, &entry ) == false )
        {
            LOGGER_INFO( "system", "Application not setup title '%s'"
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
            ->setIcon( IDI_MENGINE );

        bool fullscreen = APPLICATION_SERVICE()
            ->getFullscreenMode();

        Resolution windowResolution;
        if( APPLICATION_SERVICE()
            ->calcWindowResolution( fullscreen, &windowResolution ) == false )
        {
            return false;
        }

        if( PLATFORM_SERVICE()
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            return false;
        }

        if( APPLICATION_SERVICE()
            ->createRenderWindow() == false )
        {
            LOGGER_CRITICAL( "not create render window" );

            return false;
        }

        GAME_SERVICE()
            ->run();

        PLATFORM_SERVICE()
            ->runPlatform();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::loop()
    {
        PLATFORM_SERVICE()
            ->updatePlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::finalize()
    {
        PLATFORM_SERVICE()
            ->stopPlatform();

        BOOTSTRAPPER_SERVICE()
            ->stop();

        FARPROC proc = ::GetProcAddress( m_hInstance, "finalizeMengine" );

        FMengineFinalize dlmengineFinalize = (FMengineFinalize)proc;

        dlmengineFinalize();

        ::FreeLibrary( m_hInstance );
        m_hInstance = NULL;
    }
    //////////////////////////////////////////////////////////////////////////
}
