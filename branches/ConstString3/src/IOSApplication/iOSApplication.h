/*
 *  iOSApplication.h
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

class iOSFileLogger 
    : public LoggerInterface
{
public:
    iOSFileLogger();
    
public:
    void setVerboseLevel( EMessageLevel _level );
    bool validVerboseLevel( EMessageLevel _level ) const;
    
public:
	void log( const char * _data, int _count, EMessageLevel _level );
	void flush( void );
    
protected:
    EMessageLevel m_verboseLevel;
};
	
class iOSTimer: public TimerInterface
{
	int									start;
	mutable unsigned long				oldTime;
public:
										iOSTimer( void );
	
	virtual void						reset( void );
	virtual float						getDeltaTime( void ) const;
	virtual unsigned long				getMilliseconds( void );
	virtual unsigned long				getMicroseconds( void );
	virtual unsigned long				getMillisecondsCPU( void );
	virtual unsigned long				getMicrosecondsCPU( void );
};

class Application;
	
class iOSApplication 
    : public PlatformInterface
{
	iOSTimer							timer;
    iOSFileLogger *                     m_loggerConsole;
	Resolution							resolution;
    LogServiceInterface *               logService;
	ApplicationInterface *				application;
    
    ServiceProviderInterface * m_serviceProvider;
    
    LogServiceInterface * m_logService;
    
    WString m_userPath;
    WString m_applicationPath;
    
public:
    iOSApplication( void );
	~iOSApplication( void );
	
public:
	const bool Init( void );
	void Frame( void );
    void Finalize( void );
    
    bool OpenAL_OtherAudioIsPlaying();
    
    void TurnSoundOn();
    void TurnSoundOff();
    
public:
    ApplicationInterface * getApplication() const;
	
public:
    const WString & getCurrentPath() const;
    
public:
	void stop( void );
	const Resolution & getDesktopResolution( void ) const;
	void minimizeWindow( void );
	void setHandleMouse( bool _handle );
	void setCursorPosition( int _x, int _y );
	void showMessageBox( const WString & _message, const WString & _header, unsigned int _style );
	
	TimerInterface * getTimer( void ) const;
	
	void showKeyboard( void );
	void hideKeyboard( void );
	   
	DynamicLibraryInterface * loadDynamicLibrary( const String & _filename );
	void unloadDynamicLibrary( DynamicLibraryInterface * _lib );
	
	void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen );
	void notifyVsyncChanged( bool _vsync );
	void notifyCursorModeChanged( bool _mode );
	void notifyCursorIconSetup( const WString & _filename );
	
	void notifyCursorClipping( const Viewport & _viewport );
	void notifyCursorUnClipping( void );
    
    void notifyServiceProviderReady( ServiceProviderInterface * _serviceProvider );
    
	void setAsScreensaver( bool _set );
    
public:
    void setupLogService();
    void setupFileService();
};
	
}
