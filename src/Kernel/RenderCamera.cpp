#include "RenderCamera.h"

#include "Interface/NotificationServiceInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderCamera::RenderCamera()
        : m_invalidateProjectionMatrix( true )
        , m_invalidateViewMatrix( true )
        , m_invalidateViewProjectionMatrix( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderCamera::~RenderCamera()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderCamera::_activate()
    {
        if( Node::_activate() == false )
        {
            return true;
        }

		NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, this, &RenderCamera::notifyChangeWindowResolution );

        this->invalidateViewMatrix_();
        this->invalidateProjectionMatrix_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCamera::_deactivate()
    {
        Node::_deactivate();

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCamera::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        this->invalidateViewMatrix_();
        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCamera::updateViewProjectionMatrix_() const
    {
        m_invalidateViewProjectionMatrix = false;

        const mt::mat4f & vm = this->getCameraViewMatrix();
        const mt::mat4f & pm = this->getCameraProjectionMatrix();

        mt::mul_m4_m4( m_viewProjectionMatrix, vm, pm );
        mt::inv_m4_m4( m_viewProjectionMatrixInv, m_viewProjectionMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCamera::notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution )
    {
        MENGINE_UNUSED( _fullscreen );
        MENGINE_UNUSED( _resolution );

        this->invalidateProjectionMatrix_();
    }
}
