#	include "Config/Config.h"

#	include "MarmaladePlatform.h"

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

//#	include <mhook.h>
#	include <s3eDevice.h>
#	include <s3eWindow.h>
#	include <s3e.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Platform, Menge::MarmaladePlatform );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	MarmaladePlatform::MarmaladePlatform()
        : m_marmaladeInput(nullptr)
		, m_running(false)
		, m_pause(false)
		, m_touchpad(true)
	{
    }
    //////////////////////////////////////////////////////////////////////////
	MarmaladePlatform::~MarmaladePlatform()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
	size_t MarmaladePlatform::getCurrentPath( WChar * _path, size_t _len ) const
    {
		if( _len > 0 )
		{
			wcscpy( _path, L"" );
		}

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
	size_t MarmaladePlatform::getShortPathName( const WString & _path, WChar * _short, size_t _len ) const
    {
		size_t pathSize = _path.size();

		if( _len == pathSize )
		{
			wcscpy( _short, _path.c_str() );
		}
		
		return _path.size();
    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::getMaxClientResolution( Resolution & _resolution ) const
    {
		int32 width = s3eSurfaceGetInt( S3E_SURFACE_WIDTH );
		int32 height = s3eSurfaceGetInt( S3E_SURFACE_HEIGHT );

		_resolution.setWidth( width );
		_resolution.setHeight( height );
    }
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladePlatform::_initialize()
    {
		int32 deviceClassID = s3eDeviceGetInt( S3E_DEVICE_CLASS );

		m_touchpad = true;
		
		switch( deviceClassID )
		{
		case  S3E_DEVICE_CLASS_WINDOWS_GENERIC:
			{
				m_platformTags.clear();				
				m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "SIMULATOR" ) );
				m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "MOBILE" ) );

				m_touchpad = true;
			}break;
		case S3E_DEVICE_CLASS_OSX_DESKTOP:
			{
				m_platformTags.clear();
				m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "OSX" ) );

				m_touchpad = false;
			}break;
		case S3E_DEVICE_CLASS_IPHONE:
			{
				m_platformTags.clear();				
				m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "IOS" ) );
				m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "MOBILE" ) );

				m_touchpad = true;
			}break;
		case S3E_DEVICE_CLASS_ANDROID_GENERIC:
			{
				m_platformTags.clear();				
				m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ANDROID" ) );
				m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "MOBILE" ) );

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
	static int32 s3eCallback_UnPause( void * _systemData, void * _userData )
	{
		(void)_systemData;

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "s3eCallback_UnPause" );

		MarmaladePlatform * platform = static_cast<MarmaladePlatform *>(_userData);

		platform->changePause_( false );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static int32 s3eCallback_Pause( void * _systemData, void * _userData )
	{
		(void)_systemData;

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, "s3eCallback_Pause" );

		MarmaladePlatform * platform = static_cast<MarmaladePlatform *>(_userData);

		platform->changePause_( true );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::changePause_( bool _value )
	{
		m_pause = _value;

		mt::vec2f point;
		m_marmaladeInput->getCorrectPoint( 0, point );

		if( m_pause == true )
		{
			INPUT_SERVICE( m_serviceProvider )
				->pushMouseLeaveEvent( 0, point.x, point.y, 0.f );
		}
		else
		{
			INPUT_SERVICE( m_serviceProvider )
				->pushMouseEnterEvent( 0, point.x, point.y, 0.f );
		}

		if( m_pause == true )
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->setFocus( false );
		}
		else
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->setFocus( true );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::changeScreenSize_( const Resolution & resolution )
	{ 
		APPLICATION_SERVICE( m_serviceProvider )
			->changeWindowResolution( resolution );

		m_marmaladeInput->updateSurfaceResolution();
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::initializeMarmaladePauseCallback_()
	{
		m_pause = false;

		if( s3eDeviceRegister( S3E_DEVICE_UNPAUSE, &s3eCallback_UnPause, this ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eDeviceGetErrorString();
			s3eDeviceError err = s3eDeviceGetError();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeApplication::initialize S3E_DEVICE_UNPAUSE %s [%d]"
				, err_str
				, err
				);
		}

		if( s3eDeviceRegister( S3E_DEVICE_PAUSE, &s3eCallback_Pause, this ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eDeviceGetErrorString();
			s3eDeviceError err = s3eDeviceGetError();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeApplication::initialize S3E_DEVICE_PAUSE %s [%d]"
				, err_str
				, err
				);
		}

		if( s3eDeviceRegister( S3E_DEVICE_FOREGROUND, &s3eCallback_UnPause, this ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eDeviceGetErrorString();
			s3eDeviceError err = s3eDeviceGetError();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeApplication::initialize S3E_DEVICE_FOREGROUND %s [%d]"
				, err_str
				, err
				);
		}

		if( s3eDeviceRegister( S3E_DEVICE_BACKGROUND, &s3eCallback_Pause, this ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eDeviceGetErrorString();
			s3eDeviceError err = s3eDeviceGetError();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeApplication::initialize S3E_DEVICE_BACKGROUND %s [%d]"
				, err_str
				, err
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static int32 s3eCallback_S3E_SURFACE_SCREENSIZE( void * _systemData, void * _userData )
	{
		s3eSurfaceOrientation * orientation = static_cast<s3eSurfaceOrientation *>(_systemData);

		MarmaladePlatform * platform = static_cast<MarmaladePlatform *>(_userData);

		Resolution resolution;
		resolution.setWidth( orientation->m_Width );
		resolution.setHeight( orientation->m_Height );

		platform->changeScreenSize_( resolution );
		
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::initializeMarmaladeSurfaceScreenSizeCallback_()
	{
		if( s3eSurfaceRegister( S3E_SURFACE_SCREENSIZE, &s3eCallback_S3E_SURFACE_SCREENSIZE, this ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eSurfaceGetErrorString();
			s3eSurfaceError err = s3eSurfaceGetError();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeApplication::initialize S3E_SURFACE_SCREENSIZE %s [%d]"
				, err_str
				, err
				);
		}
	}
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::update()
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

			s3eDeviceYield( 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladePlatform::openUrlInDefaultBrowser( const WString & _url )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::_finalize()
	{
		s3eDeviceUnRegister( S3E_DEVICE_UNPAUSE, &s3eCallback_UnPause );
		s3eDeviceUnRegister( S3E_DEVICE_PAUSE, &s3eCallback_Pause );
		s3eDeviceUnRegister( S3E_DEVICE_FOREGROUND, &s3eCallback_UnPause );
		s3eDeviceUnRegister( S3E_DEVICE_BACKGROUND, &s3eCallback_Pause );

		s3eSurfaceUnRegister( S3E_SURFACE_SCREENSIZE, &s3eCallback_S3E_SURFACE_SCREENSIZE );

		if( m_marmaladeInput != nullptr )
		{
			m_marmaladeInput->finalize();
			delete m_marmaladeInput;
			m_marmaladeInput = nullptr;
		}
    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::stop()
    {
        m_running = false;
        s3eDeviceRequestQuit();
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladePlatform::createWindow( uint32_t _icon, const Menge::WString & _projectTitle, const Resolution & _resolution, bool _fullscreen )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	WindowHandle MarmaladePlatform::getWindowHandle() const
	{
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	const Tags & MarmaladePlatform::getPlatformTags() const
	{
		return m_platformTags;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladePlatform::hasTouchpad() const
	{
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::getDesktopResolution( Resolution & _resolution ) const
    {
        int32 width = s3eSurfaceGetInt( S3E_SURFACE_WIDTH );
        int32 height = s3eSurfaceGetInt( S3E_SURFACE_HEIGHT );

        _resolution.setWidth( width );
        _resolution.setHeight( height );
    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::minimizeWindow()
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::setCursorPosition( const mt::vec2f & _pos )
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::showKeyboard()
    {
		if( m_marmaladeInput != nullptr )
		{
			m_marmaladeInput->showKeyboard( true );
		}
    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::hideKeyboard()
    {
		if( m_marmaladeInput != nullptr )
		{
			m_marmaladeInput->showKeyboard( false );
		}
    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
		
    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::notifyVsyncChanged( bool _vsync )
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::notifyCursorModeChanged( bool _mode )
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
	bool MarmaladePlatform::notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer )
    {
		return true;
    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladePlatform::onEvent( const ConstString & _event, const TMapParams & _params )
    {

    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladePlatform::createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladePlatform::createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladePlatform::concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, WChar * _filePath, size_t _capacity )
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
