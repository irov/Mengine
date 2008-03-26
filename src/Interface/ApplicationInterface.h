#	pragma once

class FileSystemInterface;
typedef void* WINDOW_HANDLE;

class ApplicationListenerInterface
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
	virtual bool init( const char* _name, ApplicationListenerInterface* _listener ) = 0;
	virtual WINDOW_HANDLE createWindow( const char* _name, float _width, float _height, bool _fullscreen ) = 0;
	virtual void run() = 0;
	virtual void stop() = 0;
	virtual float getMonitorAspectRatio() = 0;
	virtual void minimizeWindow() = 0;
	virtual void notifyWindowModeChanged( float _width, float _height, bool _fullscreen ) = 0;

	virtual SystemDLLInterface* loadSystemDLL( const char* _dll ) = 0;
	virtual void unloadSystemDLL( SystemDLLInterface* ) = 0;

};


bool initInterfaceSystem( ApplicationInterface ** );
void releaseInterfaceSystem( ApplicationInterface * );