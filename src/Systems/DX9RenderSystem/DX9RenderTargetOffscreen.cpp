#include "DX9RenderTargetOffscreen.h"

#include "DX9RenderEnum.h"
#include "DX9ErrorHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    DX9RenderTargetOffscreen::DX9RenderTargetOffscreen()
        : m_surfacePlain( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderTargetOffscreen::~DX9RenderTargetOffscreen()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderTargetOffscreen::_initialize()
    {
        IDirect3DSurface9 * surfacePlain;
        IF_DXCALL( m_pD3DDevice, CreateOffscreenPlainSurface, (m_hwWidth, m_hwHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surfacePlain, NULL) )
        {
            return false;
        }

        m_surfacePlain = surfacePlain;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderTargetOffscreen::_finalize()
    {
        DXRELEASE( m_surfacePlain );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderTargetOffscreen::getData( void * const _buffer, size_t _pitch ) const
    {
        if( m_surfacePlain == nullptr )
        {
            return false;
        }

        if( m_pD3DSurface == nullptr )
        {
            return false;
        }

        DXCALL( m_pD3DDevice, GetRenderTargetData, (m_pD3DSurface, m_surfacePlain) );

        D3DLOCKED_RECT LockedRect;
        DXCALL( m_surfacePlain, LockRect, (&LockedRect, NULL, 0) );

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

        DXCALL( m_surfacePlain, UnlockRect, () );

        return true;
    }
}
