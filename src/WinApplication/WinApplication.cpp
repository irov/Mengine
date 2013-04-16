#	include "Config/Config.h"

#	include "WinApplication.h"
#	include "Menge/Application.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/InputSystemInterface.h"

#	include "WindowsLayer/VistaWindowsLayer.h"

#	include <cstdio>
#	include <clocale>
#	include <shlobj.h>

#	include "LoggerConsole.h"

#	include "FPSMonitor.h"
#	include "AlreadyRunningMonitor.h"
#	include "CriticalErrorsMonitor.h"

#	include "FileLogger.h"

#	include "resource.h"

#	include "SimpleIni.h"

#	include "Core/File.h"

#	include <ctime>
#	include <algorithm>

#	include <sstream>
#	include <iomanip>

//#	include <mhook.h>

#	include "StartupConfigLoader/StartupConfigLoader.h"

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

SERVICE_EXTERN(ServiceProvider, Menge::ServiceProviderInterface);
SERVICE_EXTERN(Application, Menge::ApplicationInterface);
SERVICE_EXTERN(StringizeService, Menge::StringizeServiceInterface);
SERVICE_EXTERN(LogService, Menge::LogServiceInterface);

SERVICE_EXTERN(ArchiveService, Menge::ArchiveServiceInterface);

SERVICE_EXTERN(ThreadSystem, Menge::ThreadSystemInterface);
SERVICE_EXTERN(ThreadService, Menge::ThreadServiceInterface);

SERVICE_EXTERN(ParticleSystem, Menge::ParticleSystemInterface);
SERVICE_EXTERN(ParticleService, Menge::ParticleServiceInterface);

SERVICE_EXTERN(RenderSystem, Menge::RenderSystemInterface);
SERVICE_EXTERN(RenderService, Menge::RenderServiceInterface);

SERVICE_EXTERN(PhysicSystem2D, Menge::PhysicSystem2DInterface);
SERVICE_EXTERN(PhysicService2D, Menge::PhysicService2DInterface);

SERVICE_EXTERN(UnicodeSystem, Menge::UnicodeSystemInterface);
SERVICE_EXTERN(UnicodeService, Menge::UnicodeServiceInterface);

SERVICE_EXTERN(FileSystem, Menge::FileSystemInterface);
SERVICE_EXTERN(FileService, Menge::FileServiceInterface);

SERVICE_EXTERN(NotificationService, Menge::NotificationServiceInterface);
SERVICE_EXTERN(ScriptService, Menge::ScriptServiceInterface);

SERVICE_EXTERN(SoundSystem, Menge::SoundSystemInterface);
SERVICE_EXTERN(SilentSoundSystem, Menge::SoundSystemInterface);
SERVICE_EXTERN(SoundService, Menge::SoundServiceInterface);

SERVICE_EXTERN(AmplifierService, Menge::AmplifierServiceInterface);
SERVICE_EXTERN(InputService, Menge::InputServiceInterface);
SERVICE_EXTERN(ConverterService, Menge::ConverterServiceInterface);
SERVICE_EXTERN(CodecService, Menge::CodecServiceInterface);
SERVICE_EXTERN(PluginService, Menge::PluginServiceInterface);

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    extern bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin );
    extern bool initPluginMengeSoundCodec( Menge::PluginInterface ** _plugin );
}

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const float s_activeFrameTime = 1000.f/60.f;
	static const float s_inactiveFrameTime = 100;
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		static void s_getOption( const Menge::String& _option
			, const Menge::String& _commandLine
			, Menge::String* _value )
		{
			if( _value == NULL )
			{
				return;
			}

			Menge::String::size_type option_index = 0;
			while( (option_index = _commandLine.find( _option, option_index )) != Menge::String::npos )
			{
				option_index += _option.length();
				if( option_index >= _commandLine.length() )
				{
					break;
				}
				char next_delim = _commandLine[option_index] == '\"' ? '\"' : ' ';
				Menge::String::size_type next_index = _commandLine.find( next_delim, option_index+1 );
				if( next_delim == '\"' )
				{
					++option_index;
				}

				Menge::String::size_type value_length = next_index - option_index;
				(*_value) += _commandLine.substr( option_index, value_length );
				_value->push_back( ' ' );
			}

			if( _value->empty() == false )
			{
				_value->erase( _value->length() - 1 );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		static bool s_hasOption( const Menge::String& _option, const Menge::String& _commandLine )
		{
			if( _commandLine.find( _option ) == Menge::String::npos )
			{
				return false;
			}

			return true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	WinApplication::WinApplication( HINSTANCE _hInstance, const String& _commandLine ) 
		: m_running(true)
		, m_active(false)
		, m_windowClassName(L"MengeWnd")
		, m_hWnd(0)
		, m_cursorInArea(false)
		, m_hInstance(_hInstance)
		, m_loggerConsole(NULL)
		, m_commandLine(" " + _commandLine + " ")
		, m_application(NULL)
		, m_fpsMonitor(0)
		, m_alreadyRunningMonitor(0)
		, m_lastMouseX(0)
		, m_lastMouseY(0)
		, m_vsync(false)
		, m_maxfps(false)
		, m_cursorMode(false)
		, m_clipingCursor(FALSE)
		, m_deadKey('\0')
		, m_logService(NULL)
        , m_fileService(NULL)
		, m_winTimer(NULL)
		, m_isDoubleClick(false)
		, m_cursor(NULL)
		, m_enableDebug(false)
		, m_inputService(0)
		, m_developmentMode(false)
        , m_noPluginMode(false)
        , m_pluginMengeImageCodec(NULL)
        , m_pluginMengeSoundCodec(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	WinApplication::~WinApplication()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TimerInterface * WinApplication::getTimer() const
	{
		return m_winTimer;
	}
    //////////////////////////////////////////////////////////////////////////
    size_t WinApplication::getShortPathName( const String & _path, char * _shortpath, size_t _shortpathlen )
    {
        WString unicode_path;
        Helper::utf8ToUnicode(m_serviceProvider, _path, unicode_path);

        wchar_t unicode_shortpath[MAX_PATH];
        DWORD unicode_shortpath_len = GetShortPathName( unicode_path.c_str(), unicode_shortpath, MAX_PATH );

        size_t utf8_shortpath_len;
        if( UNICODE_SERVICE(m_serviceProvider)
            ->unicodeToUtf8( unicode_shortpath, unicode_shortpath_len, _shortpath, _shortpathlen, &utf8_shortpath_len ) == false )
        {
            return 0;
        }
        
        return utf8_shortpath_len;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeApplicationService_()
    {   
        ApplicationInterface * application;
        if( createApplication( &application ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, application ) == false )
        {
            return false;
        }

        m_application = application;

        if( m_application->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application initialize failed" 
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeNotificationService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Notification Service..." );
        
        NotificationServiceInterface * notificationService;
        if( createNotificationService( &notificationService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, notificationService ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeThreadEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Thread Service..." );
        
        ThreadSystemInterface * threadSystem;
        if( createThreadSystem( &threadSystem ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeThreadEngine_ failed to create ThreadSystem"
                );

            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, threadSystem) == false )
        {
            return false;
        }

        threadSystem->initialize();

        ThreadServiceInterface * threadService;
        if( createThreadService( &threadService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeThreadEngine_ failed to create ThreadService"
                );

            return false;               
        }

        if( SERVICE_REGISTRY( m_serviceProvider, threadService ) == false )
        {
            return false;
        }

        m_threadService = threadService;

        if( m_threadService->initialize( 16 ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeArchiveService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Archive Service..." );

        ArchiveServiceInterface * archiveService;
        if( createArchiveService( &archiveService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeArchiveService_ failed to create ArchiveService"
                );

            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, archiveService) == false )
        {
            return false;
        }

        m_archiveService = archiveService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeFileEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing File Service..." );
        
        FileSystemInterface * fileSystem;
        if( createFileSystem( &fileSystem ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initialize failed to create FileSystem"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, fileSystem ) == false )
        {
            return false;
        }
        
        FileServiceInterface * fileService;
        if( createFileService( &fileService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initialize failed to create FileService"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, fileService ) == false )
        {
            return false;
        }

        m_fileService = fileService;

        if( m_fileService->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to initialize fileService"
                );

            return false;
        }

        if( m_enableDebug == false )
        {
            m_windowsLayer->setModuleCurrentDirectory();
        }

        if( m_windowsLayer->getCurrentDirectory( m_currentPath ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to get current directory"
                );

            return false;
        }

        m_currentPath += MENGE_FOLDER_DELIM;

        String utf8_currentPath;        
        if( Helper::unicodeToUtf8( m_serviceProvider, m_currentPath, utf8_currentPath ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to convert %ls to utf8"
                , m_currentPath.c_str()
                );

            return false;
        }

        FilePath currentPath = Helper::stringizeString( m_serviceProvider, utf8_currentPath );
        // mount root		
        if( m_fileService->mountFileSystem( ConstString::none(), currentPath, Helper::stringizeString(m_serviceProvider, "dir"), false ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to mount application directory %ls"
                , m_currentPath.c_str()
                );

            return false;
        }

        if( this->setupApplicationSetting_() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to setup application %ls"
                , m_currentPath.c_str()
                );

            return false;
        }

        //m_tempPath.clear();
        m_userPath.clear();

        if( m_developmentMode == true )
        {			
            m_userPath += m_currentPath;
            m_userPath += L"User";
            m_userPath += MENGE_FOLDER_DELIM;
        }
        else	// create user directory in ~/Local Settings/Application Data/<uUserPath>/
        {
            WChar buffer[MAX_PATH];
            LPITEMIDLIST itemIDList;

            HRESULT hr = SHGetSpecialFolderLocation( NULL,
                CSIDL_APPDATA | CSIDL_FLAG_CREATE, &itemIDList );

            if( hr != S_OK )
            {
                WString msg;
                
                if( m_windowsLayer->makeFormatMessage( hr, msg ) == false )
                {
                    LOGGER_ERROR(m_serviceProvider)("SHGetSpecialFolderLocation invalid %d"
                        , hr
                        );
                }
                else
                {
                    LOGGER_ERROR(m_serviceProvider)("SHGetSpecialFolderLocation invalid %ls '%d'"
                        , msg.c_str()
                        , hr
                        );
                }

                return false;
            }

            BOOL result = SHGetPathFromIDListW( itemIDList, buffer );

            if( result == FALSE )
            {
                LOGGER_ERROR(m_serviceProvider)("SHGetPathFromIDListW invalid"
                    );

                return false;
            }

            CoTaskMemFree( itemIDList );

            m_userPath = buffer;
            m_userPath += MENGE_FOLDER_DELIM;
            m_userPath += m_companyName;
            m_userPath += MENGE_FOLDER_DELIM;
            m_userPath += m_projectName;
            m_userPath += MENGE_FOLDER_DELIM;
        }

        String utf8_userPath;
        if( Helper::unicodeToUtf8( m_serviceProvider, m_userPath, utf8_userPath ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication: failed user directory %ls convert to ut8f"
                , m_userPath.c_str()
                );

            return false;
        }

        FilePath userPath = Helper::stringizeString( m_serviceProvider, utf8_userPath );

        // mount user directory
        if( m_fileService->mountFileSystem( Helper::stringizeString(m_serviceProvider, "user"), userPath, Helper::stringizeString(m_serviceProvider, "dir"), true ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication: failed to mount user directory %ls"
                , m_userPath.c_str()
                );

            return false;
        }
                
        std::time_t ctTime; 
        std::time(&ctTime);
        std::tm* sTime = std::localtime( &ctTime );

        WStringstream dateStream;
        dateStream << 1900 + sTime->tm_year 
            << L"_" << std::setw(2) << std::setfill(L'0') << (sTime->tm_mon+1) 
            << L"_" << std::setw(2) << std::setfill(L'0') << sTime->tm_mday 
            << L"_" << std::setw(2) << std::setfill(L'0') << sTime->tm_hour 
            << L"_" << std::setw(2) << std::setfill(L'0') << sTime->tm_min 
            << L"_"	<< std::setw(2) << std::setfill(L'0') << sTime->tm_sec;

        WString date = dateStream.str();

        WString unicode_logFilename;
        unicode_logFilename += L"Game";

        if( m_developmentMode == true )
        {
            unicode_logFilename += L"_";
            unicode_logFilename += date;
        }

        unicode_logFilename += L".log";

        String utf8_logFilename;
        if( Helper::unicodeToUtf8( m_serviceProvider, unicode_logFilename, utf8_logFilename ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication: failed log directory %ls convert to ut8f"
                , unicode_logFilename.c_str()
                );

            return false;
        }
           
        FilePath logFilename = Helper::stringizeString( m_serviceProvider, utf8_logFilename );

        OutputStreamInterface* fileLogInterface = m_fileService->openOutputFile( Helper::stringizeString(m_serviceProvider, "user"), logFilename );

        if( fileLogInterface )
        {
            m_fileLog = new FileLogger( fileLogInterface );

            m_logService->registerLogger( m_fileLog );

            LOGGER_INFO(m_serviceProvider)("WinApplication: Starting log to %s"
                , logFilename.c_str()
                );
        }

        const char * version_info = Application::getVersionInfo();

        WString logPath;
        logPath += m_userPath;
        logPath += unicode_logFilename;

        WString dumpPath;
        dumpPath += m_userPath;
        dumpPath += L"Dump";
        dumpPath += L"_";
        dumpPath += date;
        dumpPath += L".dmp";

        CriticalErrorsMonitor::run( version_info, logPath, dumpPath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeStringizeService_()
    {
        StringizeServiceInterface * stringizeService;
        if( createStringizeService( &stringizeService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, stringizeService ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeLogEngine_()
    {
        LogServiceInterface * logService;
        if( createLogService( &logService ) == false )
        {
            return false;
        }
        
        if( SERVICE_REGISTRY( m_serviceProvider, logService ) == false )
        {
            return false;
        }

        m_logService = logService;

        if( m_commandLine.find( " -console " ) != String::npos )
        {
            m_loggerConsole = new LoggerConsole(m_serviceProvider);

            EWindowsType winType = m_windowsLayer->getWindowsType();

            if( winType != EWT_98 )
            {
                m_loggerConsole->createConsole();
            }

            m_logService->registerLogger( m_loggerConsole );
        }

        EMessageLevel m_logLevel;

        if( m_developmentMode == true )
        {
            m_logLevel = LM_LOG;
        }
        else
        {
            m_logLevel = LM_ERROR;
        }

        String logLevel;
        Helper::s_getOption( " -log:", m_commandLine, &logLevel );

        if( logLevel == "0" )
        {
            m_logLevel = LM_INFO;
        }
        else if ( logLevel == "1" )
        {
            m_logLevel = LM_LOG;
        }
        else if ( logLevel == "2" )
        {
            m_logLevel = LM_WARNING;
        }
        else if ( logLevel == "3" )
        {
            m_logLevel = LM_ERROR;
        }

        m_logService->setVerboseLevel( m_logLevel );

        if( Helper::s_hasOption( " -verbose ", m_commandLine ) == true )
        {
            m_logService->setVerboseLevel( LM_MAX );

            LOGGER_INFO(m_serviceProvider)( "Verbose logging mode enabled" );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeUnicodeEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Unicode Service..." );
        
        UnicodeSystemInterface * unicodeSystem;
        if( createUnicodeSystem( &unicodeSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, unicodeSystem ) == false )
        {
            return false;
        }
        
        UnicodeServiceInterface * unicodeService;
        if( createUnicodeService( &unicodeService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, unicodeService ) == false )
        {
            return false;
        }

        m_unicodeService = unicodeService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeParticleEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Particle Service..." );
        
        ParticleSystemInterface * particleSystem;
        if( createParticleSystem( &particleSystem ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeParticleEngine_ Failed to initialize ParticleSystem"
                );

            return false;
        }
        
        if( SERVICE_REGISTRY( m_serviceProvider, particleSystem ) == false )
        {
            return false;
        }
                
        ParticleServiceInterface * particleService;
        if( createParticleService( &particleService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeParticleEngine_ Failed to initialize ParticleService"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, particleService ) == false )
        {
            return false;
        }

        m_particleService = particleService;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializePhysicEngine2D_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Physics2D Service..." );
        
        PhysicSystem2DInterface * physicSystem2D;
        if( createPhysicSystem2D( &physicSystem2D ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializePhysicEngine2D_ Failed to initialize PhysicSystem2D"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, physicSystem2D ) == false )
        {
            return false;
        }

        if( physicSystem2D->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializePhysicEngine2D_ Failed to initialize Physics System 2D"
                );

            return false;
        }
        
        PhysicService2DInterface * physicService2D;
        if( createPhysicService2D( &physicService2D ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializePhysicEngine2D_ Failed to create Physic Service 2D"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, physicService2D ) == false )
        {
            return false;
        }

        m_physicService2D = physicService2D;

        if( m_physicService2D->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializePhysicEngine2D_ Failed to initialize Physics Service 2D"
                );

            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeRenderEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Render Service..." );
        
        RenderSystemInterface * renderSystem;
        if( createRenderSystem( &renderSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, renderSystem ) == false )
        {
            return false;
        }

        if( renderSystem->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeRenderEngine_ Failed to initialize Render System"
                );

            return false;
        }

        RenderServiceInterface * renderService;
        if( createRenderService( &renderService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, renderService ) == false )
        {
            return false;
        }

        m_renderService = renderService;

        if( m_renderService->initialize( 8000 ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeRenderEngine_ Failed to initialize Render Engine"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeSoundEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Sound Service..." );

        SoundSystemInterface * soundSystem;
        if( createSoundSystem( &soundSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, soundSystem) == false )
        {
            return false;
        }

        if( soundSystem->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to initialize Sound System"
                );

            return false;
        }
        
        SoundServiceInterface * soundService;
        if( createSoundService( &soundService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to create Sound Engine"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, soundService ) == false )
        {
            return false;
        }

        m_soundService = soundService;

        if( m_soundService->initialize( false ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to initialize Sound Engine"
                );

            m_serviceProvider->unregistryService( "SoundService" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeSilentSoundEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Silent Sound Service..." );

        SoundSystemInterface * soundSystem;
        if( createSilentSoundSystem( &soundSystem ) == false )
        {
            return false;
        }
        
        if( SERVICE_REGISTRY(m_serviceProvider, soundSystem) == false )
        {
            return false;
        }

        if( soundSystem->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to initialize Silent Sound System"
                );

            return false;
        }

        SoundServiceInterface * soundService;
        if( createSoundService( &soundService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Application::initializeSilentSoundEngine_ Failed to create Sound Engine"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, soundService ) == false )
        {
            return false;
        }

        m_soundService = soundService;

        if( m_soundService->initialize( true ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSilentSoundEngine_ Failed to initialize Sound Engine"
                );

            return false;                
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeScriptEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Script Service..." );

        ScriptServiceInterface * scriptService;
        if( createScriptService( &scriptService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, scriptService ) == false )
        {
            return false;
        }

        m_scriptService = scriptService;

        if( m_scriptService->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeScriptEngine_ Failed to initialize Script Engine"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeCodecEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Codec Service..." );

        CodecServiceInterface * codecService;
        if( createCodecService( &codecService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, codecService ) == false )
        {
            return false;
        }

        m_codecService = codecService;

        m_codecService->registerCodecExt( "png", Helper::stringizeString(m_serviceProvider, "pngImage") );
        m_codecService->registerCodecExt( "jpg", Helper::stringizeString(m_serviceProvider, "jpegImage") );
        m_codecService->registerCodecExt( "jpeg", Helper::stringizeString(m_serviceProvider, "jpegImage") );

        m_codecService->registerCodecExt( "ogg", Helper::stringizeString(m_serviceProvider, "oggSound") );
        m_codecService->registerCodecExt( "hit", Helper::stringizeString(m_serviceProvider, "hitPick") );


        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeConverterEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Codec Service..." );
        
        ConverterServiceInterface * converterService;
        if( createConverterService( &converterService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, converterService ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeInputEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Input Service..." );

        InputServiceInterface * inputService;
        if( createInputService( &inputService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, inputService ) == false )
        {
            return false;
        }

        //bool result = m_inputEngine->initialize( m_serviceProvider );
        //if( result == false )
        //{
        //    MENGE_LOG_ERROR( "Input Engine initialization failed!" );
        //    return false;
        //}

        m_inputService = inputService;

        if( m_inputService->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeInputEngine_ Failed to initialize Input Engine"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializeAmplifierService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Amplifier Service..." );

        AmplifierServiceInterface * amplifierService;
        if( createAmplifierService( &amplifierService ) == false )
        {
            return false;
        }
        
        if( SERVICE_REGISTRY( m_serviceProvider, amplifierService ) == false )
        {
            return false;
        }

        m_amplifierService = amplifierService;


        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::initializePluginService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Plugin Service..." );

        PluginServiceInterface * pluginService;
        if( createPluginService( &pluginService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, pluginService) == false )
        {
            return false;
        }

        m_pluginService = pluginService;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WinApplication::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        (void)_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * WinApplication::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initialize()
	{	
        setlocale( LC_CTYPE, "" );
		//::timeBeginPeriod( 1 );
        
#	ifdef _MSC_VER
		m_enableDebug = true;
#	else
		m_enableDebug = false;
#	endif
        
        if( Helper::s_hasOption( " -dev ", m_commandLine ) == true )	// create user directory in ./User/
        {
            m_developmentMode = true;
        }

        if( Helper::s_hasOption( " -noplugin ", m_commandLine ) == true )	// create user directory in ./User/
        {
            m_noPluginMode = true;
        }

        String scriptInit;
        Helper::s_getOption( " -s:", m_commandLine, &scriptInit );

        ServiceProviderInterface * serviceProvider;
        if( createServiceProvider( &serviceProvider ) == false )
        {
            return false;
        }

        m_serviceProvider = serviceProvider;

        if( m_serviceProvider->registryService( "PlatformService", this ) == false )
        {
            return false;
        }

        m_windowsLayer = new VistaWindowsLayer();

        if( m_serviceProvider->registryService( "WindowsLayerService", m_windowsLayer ) == false )
        {
            return false;
        }

        if( this->initializeStringizeService_() == false )
        {
            return false;
        }
       
        if( this->initializeLogEngine_() == false )
        {
            return false;
        }

        if( this->initializeArchiveService_() == false )
        {
            return false;
        }
        
        if( this->initializeNotificationService_() == false )
        {
            return false;
        }

        if( this->initializeUnicodeEngine_() == false )
        {
            return false;
        }

        if( this->initializeFileEngine_() == false )
        {
            return false;
        }
        
        if( this->initializeThreadEngine_() == false )
        {
            return false;
        }
        
        if( this->initializeParticleEngine_() == false )
        {
            return false;
        }

        if( this->initializePhysicEngine2D_() == false )
        {
            return false;
        }

        if( this->initializeRenderEngine_() == false )
        {
            return false;
        }

        if( this->initializeSoundEngine_() == false )
        {
            if( this->initializeSilentSoundEngine_() == false )
            {
                return false;
            }
        }

        if( this->initializeScriptEngine_() == false )
        {
            return false;
        }
        
        if( this->initializeCodecEngine_() == false )
        {
            return false;
        }

        if( this->initializeConverterEngine_() == false )
        {
            return false;
        }

        if( this->initializeInputEngine_() == false )
        {
            return false;
        }

        if( this->initializeAmplifierService_() == false )
        {
            return false;
        }

        if( this->initializeApplicationService_() == false )
        {
            return false;
        }

        if( this->initializePluginService_() == false )
        {
            return false;
        }
                
        if( m_application->setup( m_commandLine, m_settings.applicationSettings ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application setup failed" 
                );

            return false;
        }
                
        {
            LOGGER_INFO(m_serviceProvider)( "initialize Image Codec..." );

            initPluginMengeImageCodec( &m_pluginMengeImageCodec );
            m_pluginMengeImageCodec->initialize( m_serviceProvider );
        }

        {
            LOGGER_INFO(m_serviceProvider)( "initialize Sound Codec..." );

            initPluginMengeSoundCodec( &m_pluginMengeSoundCodec );
            m_pluginMengeSoundCodec->initialize( m_serviceProvider );
        }

        if( m_noPluginMode == false )
        {
            for( TVectorWString::const_iterator
                it = m_settings.plugins.begin(),
                it_end = m_settings.plugins.end();
            it != it_end;
            ++it )
            {
                const WString & pluginName = *it;

                String utf8_pluginName;
                if( Helper::unicodeToUtf8( m_serviceProvider, pluginName, utf8_pluginName ) == false )
                {
                    LOGGER_ERROR(m_serviceProvider)("Application Failed to load plugin %ls invalid convert to utf8"
                        , pluginName.c_str()
                        ); 

                    return false;
                }

                PluginInterface * plugin = PLUGIN_SERVICE(m_serviceProvider)
                    ->loadPlugin( utf8_pluginName );

                if( plugin == NULL )
                {
                    LOGGER_ERROR(m_serviceProvider)("Application Failed to load plugin %ls"
                        , pluginName.c_str()
                        ); 

                    return false;
                }
            }

            for( TVectorWString::const_iterator
                it = m_settings.devPlugins.begin(),
                it_end = m_settings.devPlugins.end();
            it != it_end;
            ++it )
            {
                const WString & pluginName = *it;

                String utf8_pluginName;
                if( Helper::unicodeToUtf8( m_serviceProvider, pluginName, utf8_pluginName ) == false )
                {
                    LOGGER_ERROR(m_serviceProvider)("Application Failed to load dev plugin %ls invalid convert to utf8"
                        , pluginName.c_str()
                        ); 

                    continue;
                }

                PluginInterface * plugin = PLUGIN_SERVICE(m_serviceProvider)
                    ->loadPlugin( utf8_pluginName );

                if( plugin == NULL )
                {
                    LOGGER_WARNING(m_serviceProvider)("Application Failed to load dev plugin %ls"
                        , pluginName.c_str()
                        );
                }
            }
        }

        String languagePack;
        Helper::s_getOption( " -lang:", m_commandLine, &languagePack );

        if( languagePack.empty() == true )
        {
            languagePack = m_settings.defaultLocale;
        }

        LOGGER_WARNING(m_serviceProvider)("Locale %s"
            , languagePack.c_str()
            );

        if( m_application->createGame( m_settings.personalityModule, Helper::stringizeString(m_serviceProvider, languagePack), m_settings.resourcePacks, m_settings.languagePacks ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application create game failed"
                );

            return false;
        }
				
		LOGGER_INFO(m_serviceProvider)( "Application Initialize... %s"
			, m_settings.applicationSettings.platformName.c_str() 
			);
		
		const WString & projectTitle = m_application->getProjectTitle();

        if( m_settings.alreadyRunning == true )
        {	
            m_alreadyRunningMonitor = new AlreadyRunningMonitor(m_serviceProvider);

            if( m_alreadyRunningMonitor->run( EARP_SETFOCUS, m_windowClassName, projectTitle ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "Application invalid running monitor"
                    );

                return false;
            }
        }

		bool screenSaverMode = this->isSaverRunning();

		if( screenSaverMode == true )
		{
            LOGGER_ERROR(m_serviceProvider)("Application is saver running"
                );

			String lowerCmdLine = m_commandLine.substr();
			std::transform( lowerCmdLine.begin(), lowerCmdLine.end(), lowerCmdLine.begin(), std::ptr_fun( ::tolower ) );

			if( lowerCmdLine.find(" /p") != String::npos || m_commandLine.find(" -p") != String::npos )
			{
				return false;
			}
			else if( lowerCmdLine.find(" /c") != String::npos || m_commandLine.find(" -c") != String::npos )
			{
				if( m_windowsLayer->messageBox( m_hWnd, L"This screensaver has no options you can set\nDo you want to launch game?", projectTitle, MB_YESNO ) == IDNO )
				{
					return false;
				}
			}
			else
			{
				scriptInit += " screensaver";

				m_application->setFullscreenMode( true );
			}
		}

		SYSTEMTIME tm;
		GetLocalTime(&tm);
		LOGGER_INFO(m_serviceProvider)( "Date: %02d.%02d.%d, %02d:%02d:%02d"
			, tm.wDay
			, tm.wMonth
			, tm.wYear
			, tm.wHour
			, tm.wMinute
			, tm.wSecond 
			);

		OSVERSIONINFO os_ver;
		os_ver.dwOSVersionInfoSize = sizeof(os_ver);
		GetVersionEx(&os_ver);
		LOGGER_INFO(m_serviceProvider)( "OS: Windows %ld.%ld.%ld %S"
			, os_ver.dwMajorVersion
			, os_ver.dwMinorVersion
			, os_ver.dwBuildNumber 
			, os_ver.szCSDVersion
			);

		MEMORYSTATUS mem_st;
		GlobalMemoryStatus(&mem_st);
		LOGGER_INFO(m_serviceProvider)( "Memory: %ldK total, %ldK free, %ldK Page file total, %ldK Page file free"
			, mem_st.dwTotalPhys / 1024L
			, mem_st.dwAvailPhys / 1024L
			, mem_st.dwTotalPageFile / 1024L
			, mem_st.dwAvailPageFile / 1024L 
			);

		const char * versionInfo = Application::getVersionInfo();
		LOGGER_INFO(m_serviceProvider)( "SVN Revision: %s"
			, versionInfo
			);

        m_cursors[L"IDC_ARROW"] = LoadCursor( NULL, IDC_ARROW );
        m_cursors[L"IDC_UPARROW"] = LoadCursor( NULL, IDC_UPARROW );
        m_cursors[L"IDC_HAND"] = LoadCursor( NULL, IDC_HAND );
        m_cursors[L"IDC_HELP"] = LoadCursor( NULL, IDC_HELP );

        LOGGER_INFO(m_serviceProvider)( "Initializing Game data..." );
        
        if( m_application->initializeGame( m_settings.appParams, scriptInit ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application invalid initialize game"
                );

            return false;
        }
        
        LOGGER_INFO(m_serviceProvider)( "Creating Render Window..." );

        bool fullscreen = m_application->getFullscreenMode();

        //if( m_application->isValidWindowMode() == false )
        //{
        //    fullscreen = true;
        //}

        if( m_windowsLayer->setProcessDPIAware() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application not setup Process DPI Aware"
                );
        }

        Resolution windowResolution;
        m_application->calcWindowResolution( windowResolution );

        m_hWnd = this->createWindow( projectTitle, windowResolution, fullscreen );

        mt::vec2f point;
        this->getCursorPosition( point );
        m_inputService->setCursorPosition( point );

		if( m_application->createRenderWindow( m_hWnd ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)( "Application not create render window"
                );

			return false;
		}       

		m_vsync = m_application->getVSync();

		if( m_maxfps == false )
		{
			m_fpsMonitor = new FPSMonitor();
			m_fpsMonitor->initialize();

			m_fpsMonitor->setActive( true );
			m_fpsMonitor->setFrameTime( s_activeFrameTime );
		}

        HWND hWndFgnd = ::GetForegroundWindow();

        if( hWndFgnd != m_hWnd )
        {
            LOGGER_WARNING(m_serviceProvider)("Setup Foreground Window...");

            ShowWindow(m_hWnd, SW_MINIMIZE);
            ShowWindow(m_hWnd, SW_RESTORE);

            //SetWindowPos(m_hWnd,hWndFgnd,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
            //SetWindowPos(hWndFgnd,m_hWnd,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
        }
        else
        {
            ::ShowWindow( m_hWnd, SW_SHOW );
        }

        if( fullscreen == true )
        {
            Resolution desktopResolution;
            this->getDesktopResolution( desktopResolution );

            this->notifyWindowModeChanged( desktopResolution, true );
        }

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::setupApplicationSetting_()
    {
        StartupConfigLoader loader(m_serviceProvider);

        FilePath applicationPath = Helper::stringizeString( m_serviceProvider, "application.ini" );

        if( loader.load( ConstString::none(), applicationPath, m_settings ) == false )
        {
            m_windowsLayer->messageBox( NULL 
                , L"WinApplication::setupApplicationSetting_ invalid load startup config application.ini"
                , L"Config Loading Error"
                , MB_OK
                );

            return false;
        }

        m_projectName = m_settings.projectName;
        m_companyName = m_settings.companyName;
        
        String platformName;
        Helper::s_getOption( " -platform:", m_commandLine, &platformName );

        if( platformName.empty() == true )
        {
            platformName = "WIN";
        }

        m_settings.applicationSettings.platformName = platformName;

        String utf8_currentPath;
        if( Helper::unicodeToUtf8( m_serviceProvider, m_currentPath, utf8_currentPath ) == false )
        {
            m_windowsLayer->messageBox( NULL 
                , L"Invalid convert currentPath from unicode to utf8"
                , L"Config Loading Error"
                , MB_OK
                );

            return false;
        }

        m_settings.applicationSettings.baseDir = utf8_currentPath;

        if( m_projectName.empty() == true )
        {
            m_windowsLayer->messageBox( NULL
                , L"Invalid get project name from game setting"
                , L"Config Loading Error"
                , MB_OK
                );

            return false;
        }

        if( m_companyName.empty() == true )
        {
            m_windowsLayer->messageBox( NULL
                , L"Invalid get company name from game setting"
                , L"Config Loading Error"
                , MB_OK
                );

            return false;
        }
                
        //if( languagePack.empty() == true )
        //{
        //	WChar localeBuff[64];
        //	int localeBuffSize = ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, localeBuff, 64 );
        //	WString languagePackW = std::wstring(localeBuff, localeBuffSize);

        //	WindowsLayer::unicodeToAnsi( languagePackW, languagePack );

        //	std::transform( languagePack.begin(), languagePack.end(), 
        //		languagePack.begin(), std::ptr_fun( &::tolower ) );
        //}

        if( Helper::s_hasOption( " -maxfps ", m_commandLine ) == true )
        {
            m_maxfps = true;
        }

        if( m_maxfps == true )
        {
            m_settings.applicationSettings.vsync = false;
        }

        m_winTimer = new WinTimer();

        // seed randomizer
        LARGE_INTEGER randomSeed;
        ::QueryPerformanceCounter(&randomSeed);
        srand( randomSeed.LowPart );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WinApplication::getMaxClientResolution( Resolution & _resolution ) const
    {
        RECT workArea;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

        RECT clientArea = workArea;
        ::AdjustWindowRect( &clientArea, WS_OVERLAPPEDWINDOW, FALSE );
        size_t maxClientWidth = 2 * (workArea.right - workArea.left) - (clientArea.right - clientArea.left);
        size_t maxClientHeight = 2 * (workArea.bottom - workArea.top) - (clientArea.bottom - clientArea.top);

        _resolution.setWidth( maxClientWidth );
        _resolution.setHeight( maxClientHeight );
    }
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::loop()
	{
        m_winTimer->reset();

		while( m_running )
		{
			MSG  msg;
			while( m_windowsLayer->peekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				::TranslateMessage( &msg );
				
                m_windowsLayer->dispatchMessage( &msg );
			}

			float frameTime = m_winTimer->getDeltaTime();

			bool updating = m_application->onUpdate();

			if( updating == true )
			{
				m_application->onTick( frameTime );
			}
			else
			{
				Sleep(100);
			}

			if( m_vsync == false )
			{
				if( m_maxfps == false )
				{
					m_fpsMonitor->monitor();
				}
			}

            if( m_application->isFocus() == true )
            {
                if( m_application->onRender() == true )
                {
    				m_application->onFlush();
                }
			}           

            //
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::finalize()
	{
		// Clean up
		if( m_hWnd )
		{
			m_windowsLayer->destroyWindow( m_hWnd );
			m_hWnd = NULL;
		}

		if( m_hInstance )
		{
			m_windowsLayer->unregisterClass( m_windowClassName, m_hInstance );
		}	

		if( m_fpsMonitor != NULL )
		{
			m_fpsMonitor->finalize();
			delete m_fpsMonitor;
			m_fpsMonitor = 0;
		}
        
		if( m_application != NULL )
		{
			m_application->finalize();
        }

        if( m_renderService != NULL )
        {
            m_renderService->finalize();
        }

        if( m_pluginMengeImageCodec != NULL )
        {
            m_pluginMengeImageCodec->destroy();
            m_pluginMengeImageCodec = NULL;
        }

        if( m_pluginMengeSoundCodec != NULL )
        {
            m_pluginMengeSoundCodec->destroy();
            m_pluginMengeSoundCodec = NULL;
        }

        destroyInputService( m_inputService );
        destroyUnicodeService( m_unicodeService );        
        destroyFileService( m_fileService );
        destroyCodecService( m_codecService );
        destroyThreadService( m_threadService );
        destroyParticleService( m_particleService );
        destroyPhysicService2D( m_physicService2D );
        destroyRenderService( m_renderService );
        destroySoundService( m_soundService );        
        destroyAmplifierService( m_amplifierService );

        if( m_application != NULL )
        {
            m_application->destroy();

            destroyApplication( m_application );

            m_application = NULL;
        }

        {
            m_scriptService->finalize();

            destroyScriptService( m_scriptService );
        }

		if( m_fileLog != NULL )
		{
			if( m_logService )
			{
				m_logService->unregisterLogger( m_fileLog );
			}

			OutputStreamInterface * fileLogInterface = m_fileLog->getStream();
			if( fileLogInterface != NULL )
			{
				fileLogInterface->destroy();
                fileLogInterface = NULL;
			}

			delete m_fileLog;
			m_fileLog = NULL;                        
		}

		if( m_loggerConsole != NULL )
		{
			if( m_logService )
			{
				m_logService->unregisterLogger( m_loggerConsole );
			}
			
			delete m_loggerConsole;
			m_loggerConsole = NULL;
		}

        destroyLogService( m_logService );
        
		if( m_alreadyRunningMonitor != NULL )
		{
			m_alreadyRunningMonitor->stop();
			delete m_alreadyRunningMonitor;
			m_alreadyRunningMonitor = 0;
		}

		if( m_winTimer != NULL )
		{
			delete static_cast<WinTimer*>(m_winTimer);
		}

        destroyServiceProvider(m_serviceProvider);

		//::timeEndPeriod( 1 );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::stop()
	{
		m_running = false;
		if( m_hWnd != 0 )
		{
			CloseWindow( m_hWnd );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static LRESULT CALLBACK s_wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch( uMsg )
		{
		//case WM_NCCREATE:
		case WM_CREATE:
			{
                LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)lParam;

				WinApplication * app = (WinApplication *)createStruct->lpCreateParams;

                ::SetWindowLongPtr( hWnd, GWL_USERDATA, (LONG_PTR)app );

                return (LRESULT)NULL;
			}
			break;
		}

        LONG_PTR value = ::GetWindowLongPtr( hWnd, GWL_USERDATA );

		WinApplication * app = (WinApplication*)value;

		if( app == NULL )
		{
            LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

            return result;
        }
		
        LRESULT app_result = app->wndProc( hWnd, uMsg, wParam, lParam );

        return app_result;
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT CALLBACK WinApplication::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
        //print "wndProc"
      //  if( uMsg != 0x200 && uMsg != 0x84 && uMsg != 0x20 )
      //  {
		    //printf( "wndProc %x %x %x\n", uMsg, wParam, lParam );
      //  }

		switch( uMsg )
		{
		//case WM_QUERYENDSESSION:
		//	{
		//		printf("WM_QUERYENDSESSION");
		//	}break;
		case WM_ACTIVATE:
			{
                DWORD flagActive = LOWORD(wParam); // флажок активизации
                BOOL minimized = (BOOL) HIWORD(wParam); // флажок минимизации
                //HWND hwndPrevious = (HWND) lParam; // дескриптор окна 
				//bool active = (LOWORD(wParam) != WA_INACTIVE) && (HIWORD(wParam) == 0);

                bool active = (flagActive != WA_INACTIVE) && (minimized == FALSE);

				//printf("WM_ACTIVATE m_active %d %d %d\n"
    //                , active
    //                , minimized
    //                , hwndPrevious
    //                );

				//POINT cPos;
				//::GetCursorPos( &cPos );
				//::ScreenToClient( m_hWnd, &cPos );

				//mt::vec2f point;
				//point.x = static_cast<float>(cPos.x);
				//point.y = static_cast<float>(cPos.y);

				//m_application->setCursorPosition( point );
				//m_application->pushMouseMoveEvent( 0, 0, 0 );

                //printf("WinApplication::wndProc WM_ACTIVATE %d\n"
                //    , active
                //    );

				this->setActive( active );

                //::SetFocus( hWnd );

				return FALSE;
			}break;
        case WM_ACTIVATEAPP:
            {
                //BOOL active = (BOOL)wParam;

                //printf("WM_ACTIVATEAPP %d\n"
                //    , active
                //    );
            }break;
		//case WM_NCACTIVATE:
		//	{
  //              BOOL fActive = (BOOL)wParam;

  //              printf("WM_NCACTIVATE %d\n"
  //                  , fActive 
  //                  );
		//		//if( m_active )
		//		//{
		//		//	return FALSE;
		//		//}
  //              if( fActive == FALSE )
  //              {
  //                  return TRUE;
  //              }



  //              //return FALSE;
		//		//if( wParam == FALSE )
		//		//{
		//		//	return FALSE;
		//		//}
		//	}break;
        case WM_SETFOCUS:
            {
                //LOGGER_INFO(m_serviceProvider)("WM_SETFOCUS");
            }break;
        case WM_KILLFOCUS:
            {
                //LOGGER_INFO(m_serviceProvider)("WM_KILLFOCUS");
            }break;
		case WM_PAINT:
			{
				if( m_application->getFullscreenMode() == false )
				{
					m_application->onPaint();
				}
			}break;
		case WM_DISPLAYCHANGE:
			{
                LOGGER_WARNING(m_serviceProvider)("WinApplication::wndProc WM_DISPLAYCHANGE");

                //DWORD width = LOWORD(lParam); // флажок активизации
                //DWORD height = HIWORD(lParam); // флажок минимизации

                //Resolution desktopResolution;
                //desktopResolution.setWidth( width );
                //desktopResolution.setHeight( height );

                //bool fullscreenMode = m_application->getFullscreenMode();

                //this->notifyWindowModeChanged( desktopResolution, fullscreenMode );
				//m_menge->onWindowMovedOrResized();
			}break;
        case WM_SIZE:
            {
                if( wParam == SIZE_MAXIMIZED )
                {
                    m_application->setFullscreenMode( true );

                    this->setActive( true );
                }
                else if( wParam == SIZE_MINIMIZED )
                {
                    this->setActive( false );
                }
                else if( wParam == SIZE_RESTORED && m_application->getFullscreenMode() == true )
                {
                    this->setActive( true );
                }
            }break;
		case WM_GETMINMAXINFO:
			{
			// Prevent the window from going smaller than some minimu size
                MINMAXINFO * info = (MINMAXINFO*)lParam;

				info->ptMinTrackSize.x = 100;
				info->ptMinTrackSize.y = 100;
			}break;
		case WM_CLOSE:
			{
				m_application->onClose();

				return FALSE;
			}break;
		case WM_SYSKEYDOWN:
			{
                if( wParam == VK_F4 )
                {
                    this->stop();

                    return FALSE;
                }
                else if( wParam == VK_RETURN )
                {
                    bool fullscreen = m_application->getFullscreenMode();
                    m_application->setFullscreenMode( !fullscreen );

                    return FALSE;
                }
                else
                {
                    unsigned int vkc = static_cast<unsigned int>( wParam );
                    HKL  layout = ::GetKeyboardLayout(0);
                    unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

                    mt::vec2f point;
                    this->getCursorPosition(point);

                    unsigned int tvk = translateVirtualKey_( vkc, vk );

                    m_inputService->onKeyEvent( point, vkc, tvk, true );
                }                
			}break;
		case WM_SYSKEYUP:
			{
				unsigned int vkc = static_cast<unsigned int>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->getCursorPosition(point);

                unsigned int tvk = translateVirtualKey_( vkc, vk );

				m_inputService->onKeyEvent( point, vkc, tvk, false );

                //return FALSE;
			}break;
		case WM_SYSCOMMAND:
            {
                if( wParam == SC_CLOSE )
                {
                    this->stop();
                }
                //         else if( (wParam & 0xFFF0) ==  SC_RESTORE )
                //         {
                //            m_active = true;
                //         }
                //else if( (wParam & 0xFFF0) == SC_KEYMENU )
                //{
                //	if( lParam == 13 )
                //	{					
                //		bool fullscreen = m_application->getFullscreenMode();
                //		m_application->setFullscreenMode( !fullscreen );
                //	}

                //	//return FALSE;
                //}break;
            }break;
		case WM_SETCURSOR:
			//printf( "set cursor\n" );
			{
				if( m_application->isFocus() && LOWORD(lParam) == HTCLIENT && m_cursorMode == false )
				{
					::SetCursor(NULL);
				}
				else 
				{
					if (m_cursor == NULL)
					{
						m_cursor = LoadCursor(NULL, IDC_ARROW);						
					}

					::SetCursor( m_cursor );

					if( m_clipingCursor == TRUE )
					{
						::ClipCursor( &m_clipingCursorRect );
					}
				}

				return FALSE;
			}break;
		case WM_DESTROY: 

			// Post the WM_QUIT message to 
			// quit the application terminate. 

			::PostQuitMessage(0);

			return 0;
		}

        BOOL input_result;
		if( this->wndProcInput( hWnd, uMsg, wParam, lParam, input_result ) == true )
        {
            return input_result;
        }

		LRESULT result = m_windowsLayer->defWindowProc( hWnd, uMsg, wParam, lParam );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & _result )
	{
        (void)hWnd;

        bool handle = false;

		switch( uMsg )
		{
		case WM_NCMOUSEMOVE:
		case WM_MOUSELEAVE:
			{
				if( m_cursorInArea == true )
				{
					m_cursorInArea = false;

					mt::vec2f point;
					this->getCursorPosition( point );
					m_inputService->onMousePosition( 0, point );

					m_application->onAppMouseLeave();
				}

                handle = true;
                _result = FALSE;
			}break;
		case WM_MOUSEHOVER:
		case WM_MOUSEMOVE:
			{
                //::SetFocus( m_hWnd );

				mt::vec2f point;
				this->getCursorPosition( point );

				if( m_cursorInArea == false )
				{
					m_cursorInArea = true;

					m_application->onAppMouseEnter( point );

					TRACKMOUSEEVENT mouseEvent = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT };
					BOOL track = _TrackMouseEvent( &mouseEvent );

                    if( track == FALSE )
                    {
                        LOGGER_ERROR(m_serviceProvider)("_TrackMouseEvent sizeof(TRACKMOUSEEVENT), TME_LEAVE, %d, HOVER_DEFAULT"
                            , m_hWnd
                            );
                    }
				}

				int x = (int)(short)LOWORD(lParam);
				int y = (int)(short)HIWORD(lParam);
				int dx = x - m_lastMouseX;
				int dy = y - m_lastMouseY;

				if( dx == 0 && dy == 0 )
				{
					break;
				}

                float fdx = (float)dx;
                float fdy = (float)dy;

				m_inputService->onMouseMove( 0, point, fdx, fdy, 0 );

				m_lastMouseX = x;
				m_lastMouseY = y;

                handle = true;
                _result = FALSE;
			}break;
		case WM_MOUSEWHEEL:
			{
				int zDelta = static_cast<short>( HIWORD(wParam) );

				mt::vec2f point;
				this->getCursorPosition( point );				

                int wheel = zDelta / WHEEL_DELTA;

				m_inputService->onMouseMove( 0, point, 0, 0, wheel );

                handle = true;
                _result = FALSE;
			}break;
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
			{
				m_isDoubleClick = true;

                handle = true;
                _result = FALSE;
			}
			break;
		case WM_LBUTTONDOWN:
			{
				//printf("WM_LBUTTONDOWN m_active %d\n", m_active); 
                //::SetFocus( m_hWnd );

				mt::vec2f point;
				this->getCursorPosition( point );

				m_inputService->onMouseButtonEvent( 0, point, 0, true );				

                handle = true;
                _result = FALSE;
			}
			break;
		case WM_LBUTTONUP:
			{
				if( m_isDoubleClick == false )
				{
					mt::vec2f point;
					this->getCursorPosition(point);

					m_inputService->onMouseButtonEvent( 0, point, 0, false );					
				}

				m_isDoubleClick = false;

                handle = true;
                _result = FALSE;
			}break;
		case WM_RBUTTONDOWN:
			{
                //::SetFocus(m_hWnd);

				mt::vec2f point;
				this->getCursorPosition(point);

				m_inputService->onMouseButtonEvent( 0, point, 1, true );				

                handle = true;
                _result = FALSE;
			}break;
		case WM_RBUTTONUP:
			{
				if( m_isDoubleClick == false )
				{
					mt::vec2f point;
					this->getCursorPosition(point);

					m_inputService->onMouseButtonEvent( 0, point, 1, false );
				}

				m_isDoubleClick = false;

                handle = true;
                _result = FALSE;
			}break;
		case WM_MBUTTONDOWN:
			{
                //::SetFocus(m_hWnd);

				mt::vec2f point;
				this->getCursorPosition(point);

				m_inputService->onMouseButtonEvent( 0, point, 2, true );	

                handle = true;
                _result = FALSE;
			}break;
		case WM_MBUTTONUP:
			{
				mt::vec2f point;
				this->getCursorPosition(point);

				m_inputService->onMouseButtonEvent( 0, point, 2, false );	

                handle = true;
                _result = FALSE;
			}break;
		case WM_KEYDOWN:
			{
				UINT vkc = static_cast<UINT>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				UINT vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->getCursorPosition(point);
				
                UINT tvk = this->translateVirtualKey_( vkc, vk );

				m_inputService->onKeyEvent( point, vkc, tvk, true );				

                handle = true;
                _result = FALSE;
			}break;
		case WM_KEYUP:
			{
				UINT vkc = static_cast<UINT>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->getCursorPosition(point);

                UINT tvk = this->translateVirtualKey_( vkc, vk );

				m_inputService->onKeyEvent( point, vkc, tvk, false );		

                handle = true;
                _result = FALSE;
			}break;
		}

        return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	HWND WinApplication::createWindow( const Menge::WString & _projectTitle, const Resolution & _resolution, bool _fullscreen )
	{
		m_windowResolution = _resolution;
        
        HBRUSH black_brush = (HBRUSH)GetStockObject(BLACK_BRUSH);

		// Register the window class		
		ATOM result = m_windowsLayer->registerClass( 
            s_wndProc, 
            0, 
            0, 
            m_hInstance, 
            IDI_MENGE, 
            black_brush, 
            m_windowClassName
            );

		if( result == FALSE )
		{
			LOGGER_ERROR(m_serviceProvider)("Can't register window class");

			return NULL;
		}

		DWORD dwStyle = this->getWindowStyle( _fullscreen );
        dwStyle &= ~WS_VISIBLE;

		RECT rc = this->getWindowsRect( m_windowResolution, _fullscreen );

		DWORD exStyle = _fullscreen ? WS_EX_TOPMOST : 0;
        //DWORD exStyle = 0;

		HWND hwnd = m_windowsLayer->createWindowEx( exStyle, m_windowClassName, _projectTitle, dwStyle
				, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top
				, NULL, NULL, m_hInstance, (LPVOID)this );

		return hwnd; 
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
	{
        LOGGER_WARNING(m_serviceProvider)("WinApplication::notifyWindowModeChanged %d:%d %d"
            , _resolution.getWidth()
            , _resolution.getHeight()
            , _fullscreen
            );

		m_windowResolution = _resolution;

		DWORD dwStyle = this->getWindowStyle( _fullscreen );
        
		RECT rc = this->getWindowsRect( m_windowResolution, _fullscreen );
        
		DWORD dwExStyle = m_windowsLayer->getWindowLong( m_hWnd, GWL_EXSTYLE );

		if( _fullscreen == false )
		{
			// When switching back to windowed mode, need to reset window size 
			// after device has been restored
			
			m_windowsLayer->setWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle & (~WS_EX_TOPMOST) );
            m_windowsLayer->setWindowLong( m_hWnd, GWL_STYLE, dwStyle );

			::SetWindowPos(
				m_hWnd
				, HWND_NOTOPMOST
				, rc.left
				, rc.top
				, rc.right - rc.left
				, rc.bottom - rc.top
				, SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE
				);
		}
		else
		{
            m_windowsLayer->setWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle | WS_EX_TOPMOST );
            m_windowsLayer->setWindowLong( m_hWnd, GWL_STYLE, dwStyle );

			::SetWindowPos( 
				m_hWnd
				, HWND_TOPMOST
				, rc.left
				, rc.top
				, rc.right - rc.left
				, rc.bottom - rc.top
				//, SWP_FRAMECHANGED //| SWP_NOZORDER | SWP_NOACTIVATE  | SWP_FRAMECHANGED
                , SWP_NOACTIVATE
				);


            //::SetWindowPos(
            //    m_hWnd
            //    , 0
            //    , 0
            //    , 0
            //    , 0
            //    , 0
            //    , SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
            //    );
		}

		//::UpdateWindow( m_hWnd );
		//::ShowWindow( m_hWnd, SW_SHOW );
  //      ::SetFocus( m_hWnd );
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD WinApplication::getWindowStyle( bool _fullsreen ) const
	{
		DWORD dwStyle = WS_POPUP;

		if( _fullsreen == false )
		{
			dwStyle |= WS_CAPTION | WS_VISIBLE;

			bool hasWindowPanel = true;
			//m_application->getHasWindowPanel();

			if( hasWindowPanel == true )
			{
				dwStyle |= WS_SYSMENU | WS_MINIMIZEBOX;
			}

			bool allowMaximize = m_application->getAllowFullscreenSwitchShortcut();

			if( allowMaximize == true )
			{
				dwStyle |= WS_MAXIMIZEBOX;
			}
		}
		else
		{
			dwStyle |= WS_VISIBLE;
		}

		return dwStyle;
	}
	//////////////////////////////////////////////////////////////////////////
	RECT WinApplication::getWindowsRect( const Resolution & _resolution, bool _fullsreen ) const
	{
		RECT rc;
		::SetRect(&rc, 0, 0, (int)_resolution.getWidth(), (int)_resolution.getHeight());

		if( _fullsreen == false )
		{
			DWORD dwStyle = this->getWindowStyle( _fullsreen );
			::AdjustWindowRect( &rc, dwStyle, FALSE );

			RECT workArea;
			::SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

			LONG width = rc.right - rc.left;
			LONG height = rc.bottom - rc.top;

			rc.left = (workArea.left + workArea.right - width) / 2;
			rc.top = (workArea.top + workArea.bottom - height) / 2;

			rc.right = rc.left + width;
			rc.bottom = rc.top + height;
		}

		return rc;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setActive( bool _active )
	{
		if( m_active == _active )
		{
			return;
		}

		m_active = _active;

		if( m_clipingCursor == TRUE )
		{
			::ClipCursor( (m_active)?(&m_clipingCursorRect):NULL );
		}
		
		if( m_active == true )
		{
			if (m_cursor == NULL)
			{
				m_cursor = LoadCursor(NULL, IDC_ARROW);						
			}

			::SetCursor( m_cursor );
		}
		else
		{
			::SetCursor( NULL );
		}

		if( m_fpsMonitor )
		{
            if( m_active )
            {
    			m_fpsMonitor->setFrameTime( s_activeFrameTime );
            }
            else
            {
                m_fpsMonitor->setFrameTime( s_inactiveFrameTime );
            }
		}

		mt::vec2f point;
		this->getCursorPosition(point);

		m_inputService->onMousePosition( 0, point );

		m_application->onFocus( m_active, point );

		bool turnSound = m_active;
		m_application->onTurnSound( turnSound );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::getCursorPosition( mt::vec2f & _point )
	{
		POINT cPos;
		::GetCursorPos( &cPos );
		::ScreenToClient( m_hWnd, &cPos );

		_point.x = static_cast<float>(cPos.x);
		_point.y = static_cast<float>(cPos.y);
	}
	//////////////////////////////////////////////////////////////////////////
    void WinApplication::getDesktopResolution( Resolution & _resolution ) const
	{
        int cxscreen = ::GetSystemMetrics( SM_CXSCREEN );
        int cyscreen = ::GetSystemMetrics( SM_CYSCREEN );
        
        _resolution.setWidth( cxscreen );
        _resolution.setHeight( cyscreen );
	}
	//////////////////////////////////////////////////////////////////////////
	const String & WinApplication::getCurrentPath() const
	{
        static String path;
        if( Helper::unicodeToUtf8( m_serviceProvider, m_currentPath, path ) == false )
        {
            return path;
        }

		return path;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::minimizeWindow()
	{
		::ShowWindow( m_hWnd, SW_MINIMIZE );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyVsyncChanged( bool _vsync )
	{
		m_vsync = _vsync;

		if( m_fpsMonitor )
		{
			m_fpsMonitor->setActive( m_vsync == false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setHandleMouse( bool _handle )
	{
        (void)_handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setCursorPosition( int _x, int _y )
	{
		m_lastMouseX = _x;
		m_lastMouseY = _y;
		POINT cPos = { _x, _y };

		::ClientToScreen( m_hWnd, &cPos );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::showKeyboard()
	{
		//Empty - for iPad
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::hideKeyboard()
	{
		//Empty - for iPad
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorModeChanged( bool _mode )
	{
		m_cursorMode = _mode;

		if( m_cursor == NULL )
		{
			m_cursor = ::LoadCursor( NULL, IDC_ARROW );
		}

		m_cursor = m_cursorMode ? m_cursor : NULL;

		::SetCursor( m_cursor );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorClipping( const Viewport & _viewport )
	{
		POINT p1 = { static_cast<LONG>( _viewport.begin.x ),
					 static_cast<LONG>( _viewport.begin.y ) };

		POINT p2 = { static_cast<LONG>( _viewport.end.x ),
					 static_cast<LONG>( _viewport.end.y ) };

		::ClientToScreen( m_hWnd, &p1 );
		::ClientToScreen( m_hWnd, &p2 );

		m_clipingCursorRect.left = p1.x;
		m_clipingCursorRect.top = p1.y;
		m_clipingCursorRect.right = p2.x;
		m_clipingCursorRect.bottom = p2.y;

		//printf( "%d %d - %d %d\n", p1.x, p1.y, p2.x, p2.y );
		
		m_clipingCursor = ::ClipCursor( &m_clipingCursorRect );	// Bound cursor
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorUnClipping()
	{
		m_clipingCursor = FALSE;

		::ClipCursor( NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorIconSetup( const FilePath & _name, void * _buffer, size_t _size )
	{
        WString unicode_name;        
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, _name.c_str(), _name.size(), unicode_name ) == false )
        {
            return;
        }

		TMapCursors::iterator it_found = m_cursors.find( unicode_name );

		if( it_found == m_cursors.end() )
		{
			WString icoDir;

			icoDir += m_userPath;
			icoDir += L"IconCache";

            m_windowsLayer->createDirectory( icoDir );

            WString icoFile;

            icoFile += icoDir;
			icoFile += MENGE_FOLDER_DELIM;
			icoFile += unicode_name;

			WString::size_type pos1 = icoFile.find_last_of( L'/' );
			WString::size_type pos2 = icoFile.find_last_of( L'\\' );

			WString::size_type pos = (std::max)(pos1, pos2);

			WString icoDir2 = icoFile.substr( 0, pos );

			m_windowsLayer->createDirectory( icoDir2 );

			FILE * file = _wfopen( icoFile.c_str(), L"wb" );

			if( file == 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("WinApplication::notifyCursorIconSetup can't create cursor cach file %ls"
					, icoFile.c_str()
					);

				return;
			}

			fwrite( _buffer, _size, 1, file );
			fclose( file );
									
			//HCURSOR cursor = LoadCursorFromFile( _fileName.c_str() );
			HCURSOR cursor = ::LoadCursorFromFile( icoFile.c_str() );
			
			DWORD errCode = ::GetLastError();

			if( errCode != 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("WinApplication::notifyCursorIconSetup errCode %d"
					, errCode 
					);

				return;
			}
			
			it_found = m_cursors.insert( std::make_pair( unicode_name, cursor ) ).first;
		}
		
		m_cursor = it_found->second;

		::SetCursor( m_cursor );
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int WinApplication::translateVirtualKey_( unsigned int _vkc, unsigned int _vk )
	{
		if( _vk == 0 )
		{
			return 0;
		}
		
		BYTE keyState[256];
		if( ::GetKeyboardState( keyState ) == 0 )
		{
			return 0;
		}

		HKL layout = ::GetKeyboardLayout(0);

		WCHAR wide[3];
		int res = ::ToUnicodeEx( _vkc, _vk, keyState, wide, 3, 0, layout );

		if( res == -1 )
		{
			m_deadKey = wide[0];
		}
		else if( res > 0 )
		{
			return wide[0];
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::isSaverRunning() const
	{ 
		WString fileName;
		WString extention;

		if( m_windowsLayer->getModuleFileName( NULL, fileName ) == false )
        {
            return false;
        }

		if( fileName.length() < 4 )
		{
			return false;
		}

		extention = fileName.substr( fileName.length() - 4 );

		//std::transform( extention.begin(), extention.end(), extention.begin(), std::ptr_fun( ::tolower ) );
		/*for( int i=0; i<extention.length(); ++i )
		{
			extention[i] = tolower(extention[i]);
		}*/

		if( extention == L".scr" )
		{
			return true;
		}
		
		return false;
	}
	////////////////////////////////////////////////////////////////////////////
	//void WinApplication::setAsScreensaver( bool _set )
	//{
	//	if( _set == true )
	//	{
	//		WString screensaverName = m_application->getScreensaverName();

	//		FilePath fullModuleName;
	//		WindowsLayer::getModuleFileName(NULL, fullModuleName);

	//		size_t separatorPos = fullModuleName.find_last_of(MENGE_FOLDER_DELIM);
	//		FilePath binFolderPath = fullModuleName.substr(0, separatorPos);
	//		FilePath fullScreensaverPath = binFolderPath + MENGE_FOLDER_DELIM + screensaverName;

	//		FilePath fullScreensaverPathShort;
	//		WindowsLayer::getShortPathName( fullScreensaverPath, fullScreensaverPathShort );

	//		WindowsLayer::setRegistryValue( HKEY_CURRENT_USER
	//			, L"Control Panel\\Desktop"
	//			, L"SCRNSAVE.EXE"
	//			, REG_SZ
	//			, reinterpret_cast<const BYTE*>( fullScreensaverPathShort.c_str() )
	//			, fullScreensaverPathShort.length() + 1 
	//			);
	//	}
	//	else
	//	{
	//		WindowsLayer::deleteRegistryValue( HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"SCRNSAVE.EXE" );
	//	}		
	//}
    //////////////////////////////////////////////////////////////////////////
    bool WinApplication::isDevelopmentMode() const
    {
        return m_developmentMode;
    }
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::openUrlInDefaultBrowser( const WString & _url )
	{
        //size_t cmd_path_size = 0;
        //wchar_t cmd_path [MAX_PATH];
        wchar_t * cmd_path = _wgetenv( L"COMSPEC" );

        WString params = L"/c start " + _url;

        STARTUPINFO      startup_info;
        memset (& startup_info, 0, sizeof(startup_info));
        startup_info.cb = sizeof (startup_info);
        startup_info.wShowWindow = SW_HIDE;
        startup_info.dwFlags |= STARTF_USESHOWWINDOW;

        PROCESS_INFORMATION process_info;
        memset (& process_info, 0, sizeof(process_info));

        BOOL result = ::CreateProcess (
            cmd_path,          // path
            const_cast<LPWSTR>(params.c_str()), // command line
            NULL,            // process attributes
            NULL,            // thread attributes
            FALSE,            // inherit handles
            NORMAL_PRIORITY_CLASS,    // creation flags
            NULL,            // environment
            NULL,            // current directory
            & startup_info,        // startup info structure
            & process_info        // process info structure
            );

        LOGGER_WARNING(m_serviceProvider)("WinApplication::openUrlInDefaultBrowser %ls %d"
            , _url.c_str()
            , result
            );

		if( result == FALSE )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
