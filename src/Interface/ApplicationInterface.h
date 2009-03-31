#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class LogSystemInterface;

	/*class ApplicationListenerInterface
	{
	public:
		virtual void onUpdate( float _timing ) = 0;
		virtual void onFocus( bool _focus ) = 0;
		virtual void onWindowMovedOrResized() = 0;
		virtual void onClose() = 0;
		virtual void onDestroy() = 0;
		virtual void onMouseLeave() = 0;
		virtual void onMouseEnter() = 0;
		virtual bool onMouseButtonEvent( int _button, bool _isDown ) = 0;
		virtual bool onMouseMove( float _x, float _y, int _wheel ) = 0;
		virtual void onPaint() = 0;
	};*/

	class TimerInterface
	{
	public:
		virtual void reset() = 0;
		virtual float getDeltaTime() const = 0;
		virtual unsigned long getMilliseconds() = 0;
		virtual unsigned long getMicroseconds() = 0;
		virtual unsigned long getMillisecondsCPU() = 0;
		virtual unsigned long getMicrosecondsCPU() = 0;
	};

	typedef void* (*TFunctionPtr)(void);
	class DynamicLibraryInterface
    {
    public:
        virtual void load() = 0;
        virtual void unload() = 0;
		virtual const String& getName() const = 0;
        virtual TFunctionPtr getSymbol( const String& _name ) const = 0;
    };

	class ConsoleInterface 
	{
	public:
		virtual void render() = 0;
		virtual void proccessInput( unsigned int _key, unsigned int _char, bool _isDown ) = 0;
	};

	class ApplicationInterface
	{
	public:
		virtual void stop() = 0;
		virtual std::size_t getDesktopWidth() const = 0;
		virtual std::size_t getDesktopHeight() const = 0;
		virtual void minimizeWindow() = 0;
		virtual void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen ) = 0;
		virtual void setHandleMouse( bool _handle ) = 0;
		virtual void setCursorPosition( int _x, int _y ) = 0;
		virtual void showMessageBox( const String& _message, const String& _header, unsigned int _style ) = 0;

		virtual TimerInterface * getTimer() const = 0;

		virtual String ansiToUtf8( const String& _ansi ) = 0;
		virtual String utf8ToAnsi( const String& _utf8 ) = 0;

		virtual DynamicLibraryInterface* load( const String& _filename ) = 0;
		virtual void unload( DynamicLibraryInterface* _lib ) = 0;
	};
}

//bool initInterfaceSystem( Menge::ApplicationInterface ** );
//void releaseInterfaceSystem( Menge::ApplicationInterface * );
