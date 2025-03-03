#include "RenderResolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderResolution::RenderResolution()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderResolution::~RenderResolution()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderResolution::setContentResolution( const Resolution & _resolution )
    {
        m_contentResolution = _resolution;

        m_contentResolution.calcSize( &m_contentResolutionSize );
        m_contentResolution.calcSizeInv( &m_contentResolutionSizeInv );
    }
    //////////////////////////////////////////////////////////////////////////
    const Resolution & RenderResolution::getContentResolution() const
    {
        return m_contentResolution;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderResolution::setGameViewport( const Viewport & _viewport )
    {
        m_gameViewport = _viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderResolution::getGameViewport() const
    {
        return m_gameViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderResolution::fromContentToScreenPosition( const mt::vec2f & _contentPosition, mt::vec2f * const _screenPosition ) const
    {
        *_screenPosition = _contentPosition * m_contentResolutionSizeInv;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderResolution::fromScreenToContentPosition( const mt::vec2f & _screenPosition, mt::vec2f * const _contentPosition ) const
    {
        *_contentPosition = _screenPosition * m_contentResolutionSize;
    }
    //////////////////////////////////////////////////////////////////////////
}
