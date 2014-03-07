#include "WinMouseEvent.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	WinMouseEvent::WinMouseEvent()
		: m_uTimer(NULL)
		, m_hWnd(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	WinMouseEvent::~WinMouseEvent()
	{
		if( m_uTimer != NULL && m_hWnd != NULL )
		{
			::KillTimer( m_hWnd, m_uTimer );
			m_uTimer = 0;
			m_hWnd = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL WinMouseEvent::notify( HWND _hWnd )
	{
		if( m_uTimer != 0 )
		{
			return TRUE;
		}

		m_uTimer = ::SetTimer( _hWnd, UTIMER_MOUSE_EVENT, 10, NULL ); 

		if( m_uTimer != NULL )
		{
			m_hWnd = _hWnd;
			::SendMessage( _hWnd, UWM_MOUSE_ENTER, 0, 0 );
		}

		return m_uTimer != 0 ? TRUE : FALSE ;
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL WinMouseEvent::verify( HWND _hWnd )
	{
		POINT pt;
		::GetCursorPos( &pt );

		HWND wfp = WindowFromPoint( pt );
		if( _hWnd != wfp )
		{
			::KillTimer( _hWnd, m_uTimer );
			m_uTimer = 0;
			m_hWnd = NULL;
			::SendMessage( _hWnd, UWM_MOUSE_LEAVE, 0, 0 );
		}

		return TRUE;
	}
}