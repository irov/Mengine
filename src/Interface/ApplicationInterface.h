#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class ApplicationListenerInterface
	{
	public:
		virtual void onUpdate( float _timing ) = 0;
		virtual void onFocus( bool _focus ) = 0;
		virtual void onActive( bool _active ) = 0;
		virtual void onWindowMovedOrResized() = 0;
		virtual void onClose() = 0;
		virtual void onDestroy() = 0;
		virtual void onMouseLeave() = 0;
		virtual void onMouseEnter() = 0;
		virtual bool onMouseButtonEvent( int _button, bool _isDown ) = 0;
		virtual bool onMouseMove( float _x, float _y, int _wheel ) = 0;
	};

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

	class ApplicationInterface
	{
	public:
		virtual bool init( const String & _name, ApplicationListenerInterface* _listener ) = 0;
		virtual void createWindow(WindowHandle _handle) = 0;
		virtual WindowHandle createWindow( const Menge::String & _name, std::size_t _width, std::size_t _height, bool _fullscreen ) = 0;
		virtual void run() = 0;
		virtual void stop() = 0;
		virtual const std::size_t * getDesktopResolution() const = 0;
		virtual void minimizeWindow() = 0;
		virtual void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen ) = 0;
		virtual float getDeltaTime() = 0;
		virtual void setHandleMouse( bool _handle ) = 0;
		virtual void showMessageBox( const String& _message, const String& _header, unsigned int _style ) = 0;

		virtual SystemDLLInterface* loadSystemDLL( const String & _dll ) = 0;
		virtual void unloadSystemDLL( SystemDLLInterface * _dllInterface ) = 0;
	};
}

bool initInterfaceSystem( Menge::ApplicationInterface ** );
void releaseInterfaceSystem( Menge::ApplicationInterface * );