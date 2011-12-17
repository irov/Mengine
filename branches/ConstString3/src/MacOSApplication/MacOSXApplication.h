/*
 *  MacOSXApplication.h
 *  Mengine
 *
 *  Created by Dgut on 9/18/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "Interface/ApplicationInterface.h"
#include "Interface/LogSystemInterface.h"

namespace Menge
{

class MacOSXLogger : public LoggerInterface
{
public:
	virtual void						log( const void * _data, int _count, EMessageLevel _level );
	virtual void						flush( void );
};

class MacOSXLogSystem : public LogSystemInterface
{
public:
	virtual void						setVerboseLevel( EMessageLevel _level );
	virtual void						logMessage( const String & _message, EMessageLevel _level = LM_LOG );
	virtual bool						registerLogger( LoggerInterface * _logger );
	virtual void						unregisterLogger( LoggerInterface * _logger );
};
	
class MacOSXTimer: public TimerInterface
{
	int									start;
	mutable unsigned long				oldTime;
public:
										MacOSXTimer( void );
	
	virtual void						reset( void );
	virtual float						getDeltaTime( void ) const;
	virtual unsigned long				getMilliseconds( void );
	virtual unsigned long				getMicroseconds( void );
	virtual unsigned long				getMillisecondsCPU( void );
	virtual unsigned long				getMicrosecondsCPU( void );
};

class Application;
	
class MacOSXApplication 
	: public ApplicationInterface
{
	MacOSXTimer							timer;
	MacOSXLogSystem						logSystem;
	Resolution							resolution;
	Application *						application;
    
    //NSCursor *                          cursor;
public:
										MacOSXApplication( void );
										~MacOSXApplication( void );
	
	const bool							Init( void );
	void								Frame( void );
    
    //void                                FocusCursor( bool focus );
	
	virtual void						stop( void );
	virtual const Resolution &			getDesktopResolution( void ) const;
	virtual void						minimizeWindow( void );
	virtual void						setHandleMouse( bool _handle );
	virtual void						setCursorPosition( int _x, int _y );
	virtual void						showMessageBox( const String & _message, const String & _header, unsigned int _style );

	virtual TimerInterface *			getTimer( void ) const;

	virtual void						showKeyboard( void );
	virtual void						hideKeyboard( void );

	virtual void						ansiToUtf8( const String & _ansi, String & _utf8 );
	virtual void						utf8ToAnsi( const String & _utf8, String & _ansi );
	virtual void						utf8Count( const String& _utf8, std::size_t & _size );
	
	virtual DynamicLibraryInterface *	loadDynamicLibrary( const String & _filename );
	virtual void						unloadDynamicLibrary( DynamicLibraryInterface * _lib );
	
	virtual void						notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen );
	virtual void						notifyVsyncChanged( bool _vsync );
	virtual void						notifyCursorModeChanged( bool _mode );
	virtual void						notifyCursorIconSetup( const String & _filename );
	
	virtual void						notifyCursorClipping( const Viewport & _viewport );
	virtual void						notifyCursorUnClipping( void );
	virtual void						setAsScreensaver( bool _set );
};
	
}
