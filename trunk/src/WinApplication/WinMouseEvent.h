#	pragma once

#	include "WindowsLayer/WindowsIncluder.h"

#	define UWM_MOUSE_ENTER (WM_USER+0)
#	define UWM_MOUSE_LEAVE (WM_USER+1)

#	define UTIMER_MOUSE_EVENT 1

namespace Menge
{
	class WinMouseEvent
	{
	public:
		WinMouseEvent();
		~WinMouseEvent();

	public:
		BOOL verify( HWND _hWnd );
		BOOL notify( HWND _hWnd );

	public:
		void stop();

	private:	
		UINT m_uTimer;
		HWND m_hWnd;

	};
}