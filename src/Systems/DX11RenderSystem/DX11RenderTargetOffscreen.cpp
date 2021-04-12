#include "DX11RenderTargetOffscreen.h"

#include "DX11RenderEnum.h"
#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetOffscreen::DX11RenderTargetOffscreen()
        : m_pD3DSurfacePlain( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetOffscreen::~DX11RenderTargetOffscreen()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DSurfacePlain == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::_initialize()
    {
        IDirect3DSurface9 * pD3DSurfacePlain;
        IF_DXCALL( m_pD3DDevice, CreateOffscreenPlainSurface, (m_hwWidth, m_hwHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pD3DSurfacePlain, NULL) )
        {
            return false;
        }

        m_pD3DSurfacePlain = pD3DSurfacePlain;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetOffscreen::_finalize()
    {
        DX11RenderTargetTexture::_finalize();

        DXRELEASE( m_pD3DSurfacePlain );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::getData( void * const _buffer, size_t _pitch ) const
    {
        if( m_pD3DSurfacePlain == nullptr )
        {
            return false;
        }

        if( m_pD3DSurface == nullptr )
        {
            return false;
        }

        DXCALL( m_pD3DDevice, GetRenderTargetData, (m_pD3DSurface, m_pD3DSurfacePlain) );

        D3DLOCKED_RECT LockedRect;
        DXCALL( m_pD3DSurfacePlain, LockRect, (&LockedRect, NULL, 0) );

        if( LockedRect.pBits == NULL || LockedRect.Pitch == 0 )
        {
            return false;
        }

        uint8_t * dstData = reinterpret_cast<uint8_t *>(_buffer);
        uint8_t * srcData = reinterpret_cast<uint8_t *>(LockedRect.pBits);

        for( DWORD i = 0; i != m_height; ++i )
        {
            stdex::memorycopy( dstData, 0, srcData, m_width * 4 );

            dstData += _pitch;
            srcData += LockedRect.Pitch;
        }

        DXCALL( m_pD3DSurfacePlain, UnlockRect, () );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}