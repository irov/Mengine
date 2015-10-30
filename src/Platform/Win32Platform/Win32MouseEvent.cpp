#include "Win32MouseEvent.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32MouseEvent::Win32MouseEvent()
		: m_hWnd( NULL )
		, m_uTimer( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32MouseEvent::~Win32MouseEvent()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32MouseEvent::initialize( HWND _hWnd )
	{
		m_hWnd = _hWnd;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32MouseEvent::verify()
	{
		POINT pt;
		::GetCursorPos( &pt );

		HWND wfp = WindowFromPoint( pt );

		if( m_hWnd != wfp )
		{
			::KillTimer( m_hWnd, m_uTimer );
			m_uTimer = 0;

			::SendMessage( m_hWnd, UWM_MOUSE_LEAVE, 0, 0 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32MouseEvent::update()
	{
		if( m_uTimer != 0 )
		{
			return;
		}

		m_uTimer = ::SetTimer( m_hWnd, UTIMER_MOUSE_EVENT, 10, NULL );

		if( m_uTimer == 0 )
		{
			return;
		}

		::SendMessage( m_hWnd, UWM_MOUSE_ENTER, 0, 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32MouseEvent::stop()
	{
		if( m_uTimer != 0 )
		{
			::KillTimer( m_hWnd, m_uTimer );
			m_uTimer = 0;
		}
	}
}