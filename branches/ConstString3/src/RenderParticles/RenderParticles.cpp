#	include "RenderParticles.h"
#	include "ParticlesLogger.h"
#	include "RenderParticlesSuites.h"
#	include "WindowsLayer/WindowsLayer.h"

#	include "Menge/Application.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include <ctime>
#	include <cstdio>
#	include <clocale>
#	include <shlobj.h>
#	include <algorithm>
#	include <sstream>
#	include <iomanip>

//for debug 
//#	include "Menge/RenderEngine.h"
//#	include "Menge/Texture.h"

static  LRESULT CALLBACK s_wndProc(HWND hWnd, unsigned int Msg, WPARAM wParam, LPARAM lParam)
{
	return (long)DefWindowProc(hWnd, Msg, wParam, lParam);
}

namespace Menge
{
	RECT RenderParticles::getWindowsRect_( const Resolution & _resolution )
	{
		RECT rc;
		SetRect(&rc, 0, 0, (int)_resolution.getWidth(), (int)_resolution.getHeight());

		DWORD dwStyle = WS_POPUP | WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_VISIBLE;
		AdjustWindowRect( &rc, dwStyle, FALSE );

		RECT workArea;
		SystemParametersInfoA(SPI_GETWORKAREA, 0, &workArea, 0);

		LONG width = rc.right - rc.left;
		LONG height = rc.bottom - rc.top;

		rc.left = (workArea.left + workArea.right - width) / 2;
		rc.top = (workArea.top + workArea.bottom - height) / 2;

		rc.right = rc.left + width;
		rc.bottom = rc.top + height;
		return rc;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	HWND RenderParticles::createWindow_( HINSTANCE hinstance, const Menge::String & _name, const Menge::String _className, const Resolution & _resolution )
	{
		ATOM result = WindowsLayer::registerClass( s_wndProc, 0, 0, hinstance, 0
			, (HBRUSH)GetStockObject(BLACK_BRUSH)
			, _className 
			);

		if( result == FALSE )
		{
			MENGE_LOG_ERROR("Can't register window class");
			return false;
		}

		DWORD dwStyle = WS_POPUP | WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION;
		// | WS_VISIBLE
		RECT rc = this->getWindowsRect_( _resolution );

		DWORD exStyle = 0;
		HWND	hWnd; 
		hWnd = WindowsLayer::createWindowEx( exStyle, _className, _name, dwStyle
			, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top
			, NULL, NULL, hinstance, (LPVOID)this );

		return hWnd;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	RenderParticles::RenderParticles( HINSTANCE _hinstance, size_t _windowWidth, size_t _windowHeight )
		: m_hInstance(_hinstance)
		, m_renderTargetName("RenderSurface")
		, m_contentResolution(_windowWidth,_windowHeight)
		, m_resolution(_windowWidth,_windowHeight)
		, m_scriptService(NULL)
		, m_logService(NULL)
		, m_renderImage(NULL)
		, m_renderTargetImage(NULL)
		, m_renderService(NULL)
		, m_logger(NULL)
	{
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	void RenderParticles::calcRenderViewport_( Viewport & _viewport, const Resolution & _resolution )
	{
		float rw = float( _resolution.getWidth());
		float rh = float( _resolution.getHeight());


		float one_div_width = 1.f / rw;
		float one_div_height = 1.f / rh;

		float crx = float( m_contentResolution.getWidth() );
		float cry = float( m_contentResolution.getHeight() );

		float contentAspect = crx / cry;
		float aspect = rw * one_div_height;

		float dw = 1.0f;
		float dh = rw / contentAspect * one_div_height;

		if( dh > 1.0f )
		{
			dh = 1.0f;
			dw = rh * contentAspect * one_div_width;
		}

		float areaWidth = dw * rw;
		float areaHeight = dh * rh;

		_viewport.begin.x = ( rw - areaWidth ) * 0.5f;
		_viewport.begin.y = ( rh - areaHeight ) * 0.5f;
		_viewport.end.x = _viewport.begin.x + areaWidth;
		_viewport.end.y = _viewport.begin.y + areaHeight;
	}
	////////////////////////////////////////////////////////////////////////////////
	bool RenderParticles::initLogger_( FileOutputStreamInterface * _file )
	{
		wchar_t buffer[MAX_PATH];
		LPITEMIDLIST itemIDList;
		HRESULT hr = SHGetSpecialFolderLocation( NULL,
			CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, &itemIDList );
		BOOL result = SHGetPathFromIDListW( itemIDList, buffer );
		CoTaskMemFree( itemIDList );

		Menge::String logFilename;
		WindowsLayer::wstrToUtf8( Menge::WString( buffer ), logFilename );
		logFilename += "\\AEAstralaxIOPlugin";
		WindowsLayer::createDirectory( logFilename );
		logFilename += "\\log";		
				
		std::stringstream dateStream;
		std::time_t ctTime; 
		std::time(&ctTime);
		std::tm* sTime = std::localtime( &ctTime );
		dateStream << 1900 + sTime->tm_year << "_" << std::setw(2) << std::setfill('0') <<
			(sTime->tm_mon+1) << "_" << std::setw(2) << std::setfill('0') << sTime->tm_mday << "_"
			<< std::setw(2) << std::setfill('0') << sTime->tm_hour << "_" 
			<< std::setw(2) << std::setfill('0') << sTime->tm_min << "_"
			<< std::setw(2) << std::setfill('0') << sTime->tm_sec;

		String dateString = dateStream.str();
		logFilename += "_";
		logFilename += dateString;
		logFilename += ".txt";
				
		if( _file->open(logFilename) == false )
		{
			return false;
		}

		m_logger = new ParticlesLogger();
		m_logger->initialize( _file, true );
		m_logService->registerLogger( m_logger );
		m_logService->setVerboseLevel( LM_LOG );	
		return true;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	bool RenderParticles::initialise( )
	{
		initInterfaceSystem(&m_application);
		
		Menge::String platformName = "WIN";

		if( m_application->initialize( this, platformName, "" ) == false )
		{
			return false;
		}
		
		//////////////////////////////////////////////////////////

		String applicationPath;
		WindowsLayer::getCurrentDirectory( applicationPath );

		applicationPath += "\\";

		ServiceProviderInterface * serviceProvider = m_application->getServiceProvider();
		FileServiceInterface * fileService = serviceProvider->getServiceT<FileServiceInterface>("FileService");
		
		// mount root		
		if( fileService->mountFileSystem( ConstString(""), "", ConstString("dir"), false ) == false )
		{
			return false;
		}

		/////////////////////////////////////////////////////////

		m_scriptService = serviceProvider->getServiceT<ScriptServiceInterface>("ScriptService");

		//////////////////////////////////////////////////////////
		
		FileOutputStreamInterface * logFile = fileService->createOutputFile( ConstString("") );
		
		m_logService = serviceProvider->getServiceT<LogServiceInterface>("LogService");
		
		if( this->initLogger_( logFile ) == false )
		{
			return false;
		}
		
		//////////////////////////////////////////////////////////
		
		m_renderService = serviceProvider->getServiceT<RenderServiceInterface>("RenderService");

		m_renderhWnd = this->createWindow_(m_hInstance, "RenderWindow","RenderWindowClass", m_resolution );

		m_renderService->setVSync(true);

		int bits = 32;
		int FSAAType = 0;
		int FSAAQuality = 0;
		bool textureFiltering = true;

		Viewport renderViewport;
		this->calcRenderViewport_( renderViewport, m_resolution );

		bool isCreatedRenderWindow = m_renderService->createRenderWindow( m_resolution, m_contentResolution
			, renderViewport, bits, false,
			m_renderhWnd, FSAAType, FSAAQuality );

		if( isCreatedRenderWindow == false )
		{
			return false;
		}

		//::ShowWindow( m_renderhWnd, SW_SHOW );
		//////////////////////////////////////////////////////////
		
		ConstString name("RenderTarget");
		m_renderImage = m_renderService->createTexture( name, m_resolution.getWidth(), m_resolution.getHeight(), PF_A8R8G8B8 );
		m_renderTargetImage = m_renderService->createRenderTargetTexture( m_renderTargetName, m_resolution.getWidth(), m_resolution.getHeight(), PF_A8R8G8B8 );
		
		m_renderService->setRenderTargetTexture( m_renderTargetImage ,true);
		return true;	
	}
	//////////////////////////////////////////////////////////////////////////////
	void RenderParticles::finalize()
	{
		m_renderService->releaseTexture(m_renderImage);
		m_renderService->releaseTexture(m_renderTargetImage);
	}
	////////////////////////////////////////////////////////////////////////////////
	void RenderParticles::beginRender()
	{
		m_renderService->clear(0);
		m_renderService->beginScene();
	}
	////////////////////////////////////////////////////////////////////////////////
	void RenderParticles::endRender()
	{
		m_renderService->endScene();
		m_renderService->swapBuffers();
	}
	////////////////////////////////////////////////////////////////////////////////
	void RenderParticles::getRenderData( RenderParticlesFrame * _frame )
	{
		m_renderService->screenshot( m_renderImage, mt::vec4f( 0, 0, _frame->width, _frame->height ) );
		//RenderEngine::get()->saveImage( m_renderImage,ConstString(""),String("d:\\temp.png"));
		int pitch;
		Rect rect( 0, 0, _frame->width, _frame->height );
		unsigned char * source = m_renderImage->lockRect( &pitch, rect, true );
				
		/*
		FILE * fp = fopen("d://test.raw","w+");
		fwrite( source,1, pitch * _frame->height, fp );
		fclose(fp);
		*/
		
		unsigned char * dest = _frame->data;
		size_t index;
		for(size_t y = 0; y < _frame->height; y++)
		{
			for(size_t x = 0; x < _frame->width; x++)
			{
				index = x*4;
				size_t a = source[index + 3];
				size_t r = source[index + 2];
				size_t g = source[index + 1];
				size_t b = source[index + 0];
				/*if(a == 0)
				{
					a = 0xffffff;
				}*/
				/*if( a == 0 	&& ( r != 0 || g != 0 || b != 0  ) )
				{
					a = 0xffffff;
				}*/
				a = 0xffffff;
				//reverse pixels in directx buffer	
				dest[index + 0] = a;
				dest[index + 1] = r;
				dest[index + 2] = g;
				dest[index + 3] = b;
				
				/*
				dest[index + 0] = source[index + 3];
				dest[index + 1] = source[index + 2];
				dest[index + 2] = source[index + 1];
				dest[index + 3] = source[index + 0];
				*/
			}
			dest += _frame->pitch;
			source += pitch;
		}
		// Read data here!
		
		m_renderImage->unlock();
	}
	////////////////////////////////////////////////////////////////////////////////
	void RenderParticles::translateMessage()
	{
		MSG  msg;
		while( WindowsLayer::peekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			::TranslateMessage( &msg );
			::WindowsLayer::dispatchMessage( &msg );
		}
	}
	////////////////////////////////////////////////////////////////////////////////
	void RenderParticles::log( const String& _message )
	{
		m_logService->logMessage( LM_LOG, _message );
	}
	////////////////////////////////////////////////////////////////////////////////
	ScriptServiceInterface * RenderParticles::getScriptService()
	{
		return m_scriptService;
	}
	////////////////////////////////////////////////////////////////////////////////
}