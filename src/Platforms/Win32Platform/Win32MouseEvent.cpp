#include "Win32MouseEvent.h"

#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"

namespace Mengine
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
    void Win32MouseEvent::setHWND( HWND _hWnd )
    {
        m_hWnd = _hWnd;
    }
    //////////////////////////////////////////////////////////////////////////
    HWND Win32MouseEvent::getHWND() const
    {
        return m_hWnd;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MouseEvent::verify()
    {
        if( m_hWnd == NULL )
        {
            return;
        }

        if( m_uTimer == 0 )
        {
            return;
        }

        POINT pt;
        if( ::GetCursorPos( &pt ) == FALSE )
        {
            return;
        }

        RECT rect;
        if( ::GetClientRect( m_hWnd, &rect ) == FALSE )
        {
            return;
        }

        POINT pleft;
        pleft.x = rect.left;
        pleft.y = rect.top;
        if( ::ClientToScreen( m_hWnd, &pleft ) == FALSE )
        {
            return;
        }

        POINT pright;
        pright.x = rect.right;
        pright.y = rect.bottom;
        if( ::ClientToScreen( m_hWnd, &pright ) == FALSE )
        {
            return;
        }

        RECT rclient;
        rclient.left = pleft.x;
        rclient.top = pleft.y;
        rclient.right = pright.x;
        rclient.bottom = pright.y;

        if( ::PtInRect( &rclient, pt ) == FALSE )
        {
            ::KillTimer( m_hWnd, m_uTimer );
            m_uTimer = 0;

            ::SendMessage( m_hWnd, MENGINE_UWM_MOUSE_LEAVE, 0, 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MouseEvent::update()
    {
        if( m_hWnd == NULL )
        {
            return;
        }

        if( m_uTimer != 0 )
        {
            return;
        }

        UINT uElapse = 20;

        UINT_PTR uTimer = ::SetTimer( m_hWnd, MENGINE_UTIMER_MOUSE_EVENT, uElapse, NULL );

        if( uTimer == 0 )
        {
            LOGGER_ERROR( "SetTimer [%u] get error %ls"
                , uElapse
                , Helper::Win32GetLastErrorMessageW()
            );

            return;
        }

        m_uTimer = uTimer;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MouseEvent::stop()
    {
        if( m_hWnd == NULL )
        {
            return;
        }

        if( m_uTimer != 0 )
        {
            ::KillTimer( m_hWnd, m_uTimer );
            m_uTimer = 0;
        }

        m_hWnd = NULL;
    }
    //////////////////////////////////////////////////////////////////////////
}