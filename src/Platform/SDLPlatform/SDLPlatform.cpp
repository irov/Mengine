#	include "Config/Config.h"

#	include "SDLPlatform.h"

#	include "Interface/LoggerInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/InputSystemInterface.h"
#	include "Interface/OptionsInterface.h"
#	include "Interface/TimerInterface.h"

#	include <cstdio>
#	include <clocale>

#	include "Core/FileLogger.h"
#	include "Core/IniUtil.h"

#	include "Factory/FactorableUnique.h"
#	include "Factory/FactoryDefault.h"

#	include "Logger/Logger.h"

//#	include "resource.h"

#	include <ctime>
#	include <algorithm>

#	include <sstream>
#	include <iomanip>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Platform, Menge::SDLPlatform );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	SDLPlatform::SDLPlatform()
        : m_running(false)
		, m_pause(false)
		, m_touchpad(true)
	{
    }
    //////////////////////////////////////////////////////////////////////////
	SDLPlatform::~SDLPlatform()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
	size_t SDLPlatform::getCurrentPath( WChar * _path, size_t _len ) const
    {
		if( _len > 0 )
		{
			wcscpy( _path, L"" );
		}

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
	size_t SDLPlatform::getShortPathName( const WString & _path, WChar * _short, size_t _len ) const
    {
		size_t pathSize = _path.size();

		if( _len == pathSize )
		{
			wcscpy( _short, _path.c_str() );
		}
		
		return _path.size();
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::getMaxClientResolution( Resolution & _resolution ) const
    {
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLPlatform::_initialize()
    {
		int32 deviceClassID = s3eDeviceGetInt( S3E_DEVICE_CLASS );

		m_touchpad = true;
		
		switch( deviceClassID )
		{
		case  S3E_DEVICE_CLASS_WINDOWS_GENERIC:
			{
				m_platformName = STRINGIZE_STRING_LOCAL(m_serviceProvider, "SIMULATOR");
				m_touchpad = true;
			}break;
		case S3E_DEVICE_CLASS_OSX_DESKTOP:
			{
				m_platformName = STRINGIZE_STRING_LOCAL(m_serviceProvider, "OSX");
				m_touchpad = false;
			}break;
		case S3E_DEVICE_CLASS_IPHONE:
			{
				m_platformName = STRINGIZE_STRING_LOCAL(m_serviceProvider, "IOS");
				m_touchpad = true;
			}break;
		case S3E_DEVICE_CLASS_ANDROID_GENERIC:
			{
				m_platformName = STRINGIZE_STRING_LOCAL(m_serviceProvider, "ANDROID");
				m_touchpad = true;
			}break;
		default:
			break;
		};
				
		LOGGER_WARNING(m_serviceProvider)("Device info:"
			);

		const char * deviceID = s3eDeviceGetString( S3E_DEVICE_ID );

		LOGGER_WARNING(m_serviceProvider)("ID: %s"
			, deviceID
			);

		const char * deviceOS = s3eDeviceGetString( S3E_DEVICE_OS );

		LOGGER_WARNING(m_serviceProvider)("OS: %s"
			, deviceOS
			);

		const char * deviceOSVersion= s3eDeviceGetString( S3E_DEVICE_OS_VERSION );

		LOGGER_WARNING(m_serviceProvider)("OS Version: %s"
			, deviceOSVersion
			);

		const char * deviceClass = s3eDeviceGetString( S3E_DEVICE_CLASS );

		LOGGER_WARNING(m_serviceProvider)("Class: %s"
			, deviceClass
			);

		const char * deviceArchitecture = s3eDeviceGetString( S3E_DEVICE_ARCHITECTURE );

		LOGGER_WARNING(m_serviceProvider)("Architecture: %s"
			, deviceArchitecture
			);

		const char * deviceName = s3eDeviceGetString( S3E_DEVICE_NAME );

		LOGGER_WARNING(m_serviceProvider)("Name: %s"
			, deviceName
			);		
        
		m_marmaladeInput = new MarmaladeInput;
		m_marmaladeInput->setServiceProvider( m_serviceProvider );
		
		if( m_marmaladeInput->initialize() == false )
		{
			return false;
		}

#	ifndef _DEBUG		
		s3ePointerSetInt( S3E_POINTER_HIDE_CURSOR, 1 );
#	endif

		this->initializeMarmaladePauseCallback_();
		this->initializeMarmaladeSurfaceScreenSizeCallback_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::update()
    {
		TIMER_SERVICE( m_serviceProvider )
			->resetDeltaTime();

        while( true )
        {
            s3eBool quit = s3eDeviceCheckQuitRequest();

            if( quit == S3E_TRUE )
            {
                break;
            }

			float frameTime = TIMER_SERVICE(m_serviceProvider)
				->getDeltaTime();

			if( m_pause == true )
			{
				s3eDeviceYield( 100 );

				continue;
			}
            
            if( APPLICATION_SERVICE(m_serviceProvider)
				->isFocus() == true )
            {
                s3eDeviceBacklightOn();
            }

            m_marmaladeInput->update();
            
			bool updating = APPLICATION_SERVICE( m_serviceProvider )
				->beginUpdate();

            if( updating == true )
            {                
				APPLICATION_SERVICE( m_serviceProvider )
					->tick( frameTime );
            }
            else
            {
                s3eDeviceYield( 20 );
            }

			if( APPLICATION_SERVICE( m_serviceProvider )
				->isFocus() == true )
			{
				if( APPLICATION_SERVICE( m_serviceProvider )
					->render() == true )
				{
					APPLICATION_SERVICE( m_serviceProvider )
						->flush();
				}
			}

			APPLICATION_SERVICE( m_serviceProvider )
				->endUpdate();

			s3eDeviceYield( 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLPlatform::openUrlInDefaultBrowser( const WString & _url )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::_finalize()
	{

    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::stop()
    {
        m_running = false;
        s3eDeviceRequestQuit();
    }
	//////////////////////////////////////////////////////////////////////////
	bool SDLPlatform::createWindow( uint32_t _icon, const Menge::WString & _projectTitle, const Resolution & _resolution, bool _fullscreen )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	WindowHandle SDLPlatform::getWindowHandle() const
	{
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & SDLPlatform::getPlatformName() const
	{
		return m_platformName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLPlatform::isTouchpad() const
	{
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::getDesktopResolution( Resolution & _resolution ) const
    {
        int32 width = s3eSurfaceGetInt( S3E_SURFACE_WIDTH );
        int32 height = s3eSurfaceGetInt( S3E_SURFACE_HEIGHT );

        _resolution.setWidth( width );
        _resolution.setHeight( height );
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::minimizeWindow()
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::setCursorPosition( const mt::vec2f & _pos )
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::showKeyboard()
    {
		if( m_marmaladeInput != nullptr )
		{
			m_marmaladeInput->showKeyboard( true );
		}
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::hideKeyboard()
    {
		if( m_marmaladeInput != nullptr )
		{
			m_marmaladeInput->showKeyboard( false );
		}
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
		
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::notifyVsyncChanged( bool _vsync )
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::notifyCursorModeChanged( bool _mode )
    {
		if( _mode == true )
		{
			s3ePointerSetInt( S3E_POINTER_HIDE_CURSOR, 0 );
		}
		else
		{
			s3ePointerSetInt( S3E_POINTER_HIDE_CURSOR, 1 );
		}		
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLPlatform::notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer )
    {
		return true;
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLPlatform::onEvent( const ConstString & _event, const TMapParams & _params )
    {

    }
	//////////////////////////////////////////////////////////////////////////
	bool SDLPlatform::createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLPlatform::createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLPlatform::concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, WChar * _filePath, size_t _capacity )
	{
		size_t folderSize = _folder.size();
		size_t dirSize = _fileName.size();

		size_t filePathSize = folderSize + dirSize;

		if( filePathSize >= MENGINE_MAX_PATH )
		{
			return false;
		}

		Char filePath[MENGINE_MAX_PATH];
		stdex::memorycopy( filePath, 0, _folder.c_str(), folderSize );
		stdex::memorycopy( filePath, folderSize, _fileName.c_str(), dirSize );

		filePath[filePathSize] = '\0';
		//filePathSize += 1; //Null

		if( UNICODE_SERVICE( m_serviceProvider )
			->utf8ToUnicode( filePath, filePathSize, _filePath, _capacity, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
}
