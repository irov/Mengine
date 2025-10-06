#include "RenderResolution.h"

#include "Kernel/NotificationHelper.h"

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
    bool RenderResolution::_activate()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, &RenderResolution::notifyChangeWindowResolution_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderResolution::_deactivate()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION );
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
    void RenderResolution::notifyChangeWindowResolution_( bool _fullscreen, const Resolution & _resolution )
    {
          MENGINE_UNUSED( _fullscreen );
          MENGINE_UNUSED( _resolution );
    }
    //////////////////////////////////////////////////////////////////////////
}
