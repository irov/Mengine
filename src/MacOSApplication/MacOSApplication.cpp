/*
 *  MacOSApplication.cpp
 *  Mac
 *
 *  Created by Yuriy Levchenko on 22.10.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "Config/Config.h"

#include "MacOSApplication.h"
#include "Menge/Application.h"

#include "Interface/LogSystemInterface.h"

const Menge::TCharA * config_file = "application.xml";

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
		if( m_logSystem ) m_logSystem->logMessage( message + StringA("\n"), LM_LOG );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + StringA("\n"), LM_ERROR );

namespace Menge
{
	MacOSApplication::MacOSApplication( const StringA& _commandLine )
		: m_commandLine( _commandLine )
		, m_menge( NULL )
		, m_logSystem( NULL )
	{
	}
	
	MacOSApplication::~MacOSApplication()
	{
	}
	
	bool MacOSApplication::start()
	{
		m_menge = new Application( this );
		if( m_menge == NULL )
		{
			return false;
		}
		
		m_logSystem = m_menge->initializeLogSystem();
		
		m_logSystem->setVerboseLevel( Menge::LM_MAX );
		LOG( "Initializing Mengine..." );
		if( m_menge->initialize( config_file, m_commandLine.c_str(), true ) == false )
		{
			return false;
		}
	
		
		return true;
	}
	
	void MacOSApplication::loop()
	{
	}
	
	void MacOSApplication::stop()
	{
	}
	
	WindowHandle MacOSApplication::createWindow( const Menge::String& _name, std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		return 0;
	}
	
	std::size_t MacOSApplication::getDesktopWidth() const
	{
		return 0;
	}
	
	std::size_t MacOSApplication::getDesktopHeight() const
	{
		return 0;
	}
	
	void MacOSApplication::minimizeWindow()
	{
	
	}
	
	void MacOSApplication::setDesktopResolution( std::size_t _width, std::size_t _height )
	{
	}
	
	void MacOSApplication::notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
	}
	
	float MacOSApplication::getDeltaTime()
	{
		return 0.0f;
	}
	
	SystemDLLInterface* MacOSApplication::loadSystemDLL( const String& _dll )
	{
		return NULL;
	}
	
	void MacOSApplication::unloadSystemDLL( SystemDLLInterface* _interface )
	{
	}
	
	void MacOSApplication::setHandleMouse( bool _handle )
	{
	}
	
	void MacOSApplication::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
	}
}	// namespace Menge