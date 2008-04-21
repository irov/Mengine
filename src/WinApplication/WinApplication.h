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

#	include <Windows.h>
#	include <string>

class SystemDLL;

class WinApplication
	: public ApplicationInterface
{
public:
	WinApplication();
	~WinApplication();

public:
	bool init( const char* _name, ApplicationListenerInterface* _listener ) override;
	void run() override;
	void stop()	override;
	WINDOW_HANDLE createWindow( const char* _name, float _width, float _height, bool _fullscreen ) override;
	float getMonitorAspectRatio() override;
	void minimizeWindow() override;
	void setPrimaryMonitorAspect( float _aspect );
	void notifyWindowModeChanged( float _width, float _height, bool _fullscreen ) override;
	//void changeResolution( int _width, int _height, int _bits, bool _fullscreen );
	SystemDLLInterface* loadSystemDLL( const char* _dll ) override;
	void unloadSystemDLL(SystemDLLInterface* _interface ) override;

public:
	LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	ApplicationListenerInterface* m_listener;
	LARGE_INTEGER m_timerFrequency;
	LARGE_INTEGER m_lastTime;
	bool	m_running;
	bool	m_active;
	float	m_frameTime;
	HWND	m_hWnd;
	bool	m_cursorInArea;
	WINDOWINFO m_wndInfo;
	HANDLE m_mutex;	// for multiple instance tracking
	bool m_focus;
	std::string m_name;
	float m_winWidth;
	float m_winHeight;
	bool	m_fullscreen;

	float m_primaryMonitorAspect;

};
