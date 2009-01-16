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

	class SystemDLLInterface
	{
	public:
		template<class T>
		T* getInterface()
		{
			typedef bool (*FInterfaceInitial)(T**);
			typedef void (*FInterfaceRelease)(T*);

			T* _interface;
			((FInterfaceInitial)m_init)( &_interface );
			return _interface;
		}
	protected:
		typedef int (*FInterface)();

		FInterface m_init;
		FInterface m_fini;

	};

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

	class ApplicationInterface
	{
	public:
		virtual WindowHandle createWindow( const Menge::String & _name, std::size_t _width, std::size_t _height, bool _fullscreen ) = 0;
		virtual void stop() = 0;
		virtual std::size_t getDesktopWidth() const = 0;
		virtual std::size_t getDesktopHeight() const = 0;
		virtual void minimizeWindow() = 0;
		virtual void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen ) = 0;
		virtual void setHandleMouse( bool _handle ) = 0;
		virtual void showMessageBox( const String& _message, const String& _header, unsigned int _style ) = 0;

		virtual SystemDLLInterface* loadSystemDLL( const String & _dll ) = 0;
		virtual void unloadSystemDLL( SystemDLLInterface * _dllInterface ) = 0;
		virtual TimerInterface * getTimer() const = 0;

		virtual String ansiToUtf8( const String& _ansi ) = 0;
		virtual String utf8ToAnsi( const String& _utf8 ) = 0;
	};
}

//bool initInterfaceSystem( Menge::ApplicationInterface ** );
//void releaseInterfaceSystem( Menge::ApplicationInterface * );
