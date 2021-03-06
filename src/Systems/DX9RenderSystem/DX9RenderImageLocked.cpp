#include "DX9RenderImageLocked.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderImageLocked::DX9RenderImageLocked()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderImageLocked::~DX9RenderImageLocked()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImageLocked::initialize( const D3DLOCKED_RECT & _lockedRect )
    {
        m_lockedRect = _lockedRect;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer DX9RenderImageLocked::getBuffer( size_t * const _pitch ) const
    {
        *_pitch = (size_t)m_lockedRect.Pitch;

        return m_lockedRect.pBits;
    }
    //////////////////////////////////////////////////////////////////////////
}