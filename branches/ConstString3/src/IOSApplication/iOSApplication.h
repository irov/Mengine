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

class iOSLogger : public LoggerInterface
{
public:
	void log( const void * _data, int _count, EMessageLevel _level );
	void flush( void );
};

class iOSLogSystem : public LogSystemInterface
{
public:
    iOSLogSystem();
    
protected:
	void setVerboseLevel( EMessageLevel _level );
    bool validVerboseLevel( EMessageLevel _level ) const; 
	void logMessage( EMessageLevel _level, const String & _message );
	bool registerLogger( LoggerInterface * _logger );
	void unregisterLogger( LoggerInterface * _logger );
    
protected:
    EMessageLevel m_level;
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
    : public ApplicationInterface
{
	iOSTimer							timer;
	iOSLogSystem						logSystem;
	Resolution							resolution;
	Application *						application;
    
public:
    iOSApplication( void );
	~iOSApplication( void );
	
public:
	const bool Init( void );
	void Frame( void );
    
    void AudioSessionBeginInterruption();
    void AudioSessionEndInterruption();
    
    void TurnSoundOn();
    void TurnSoundOff();
    
public:
    Application * getApplication() const;
	
public:
	virtual void stop( void );
	virtual const Resolution & getDesktopResolution( void ) const;
	virtual void minimizeWindow( void );
	virtual void setHandleMouse( bool _handle );
	virtual void setCursorPosition( int _x, int _y );
	virtual void showMessageBox( const String & _message, const String & _header, unsigned int _style );
	
	virtual TimerInterface * getTimer( void ) const;
	
	virtual void showKeyboard( void );
	virtual void hideKeyboard( void );
	
	virtual void ansiToUtf8( const String & _ansi, String & _utf8 );
	virtual void utf8ToAnsi( const String & _utf8, String & _ansi );
    void utf8Count( const String& _utf8, std::size_t & _size );
    
	virtual DynamicLibraryInterface * loadDynamicLibrary( const String & _filename );
	virtual void unloadDynamicLibrary( DynamicLibraryInterface * _lib );
	
	virtual void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen );
	virtual void notifyVsyncChanged( bool _vsync );
	virtual void notifyCursorModeChanged( bool _mode );
	virtual void notifyCursorIconSetup( const String & _filename );
	
	virtual void notifyCursorClipping( const Viewport & _viewport );
	virtual void notifyCursorUnClipping( void );
    
    virtual void notifySoundInitialize();
    
	virtual void setAsScreensaver( bool _set );
};
	
}
