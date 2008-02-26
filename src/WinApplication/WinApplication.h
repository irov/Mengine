#	pragma once

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#	include "Interface/ApplicationInterface.h"

//#	include "Ogre.h"

//#	include <list>
#	include <Windows.h>
#	include <string>

class SystemDLL;

class WinApplication
	: public ApplicationInterface
	//, public Ogre::FrameListener
{
public:
	WinApplication();
	~WinApplication();

public:
	bool init( const char* _name, const char * _args, ApplicationListenerInterface* _listener ) override;
	void run() override;
	void stop()	override;
	WINDOW_HANDLE createWindow( const char* _name, unsigned int _width, unsigned int _height, bool _fullscreen ) override;
	FileSystemInterface* getFileSystemInterface() override;
	SystemDLLInterface* loadSystemDLL( const char* _dll ) override;
	void unloadSystemDLL(SystemDLLInterface* _interface ) override;
	float getMonitorAspectRatio() override;
	void minimizeWindow() override;
	//void changeResolution( int _width, int _height, int _bits, bool _fullscreen );

//protected:

	//bool frameStarted( const Ogre::FrameEvent & evt) override;
	//bool frameEnded( const Ogre::FrameEvent & evt) override;

private:
	static ApplicationListenerInterface* m_listener;
	LARGE_INTEGER m_timerFrequency;
	LARGE_INTEGER m_lastTime;
	bool	m_running;
	static bool	m_active;
	float	m_frameTime;
	static HWND	m_hWnd;
	static bool	m_cursorInArea;
	static WINDOWINFO m_wndInfo;
	HANDLE m_mutex;	// for multiple instance tracking
	bool m_focus;
	std::string m_name;

	static LRESULT CALLBACK _wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	static BOOL CALLBACK _monitorEnumProc( HMONITOR _hMonitor, HDC _hdc, LPRECT, LPARAM );
	static float m_primaryMonitorAspect;

	SystemDLLInterface* m_fileSystemDLL;
	FileSystemInterface* m_fileSystem;
};
