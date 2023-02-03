#include "RenderCamera.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"

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
        MENGINE_ASSERTION_OBSERVABLE( this, "camera '%s'"
            , this->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderCamera::_activate()
    {
        if( Node::_activate() == false )
        {
            return true;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, &RenderCamera::notifyChangeWindowResolution_, MENGINE_DOCUMENT_FACTORABLE );

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
    void RenderCamera::_invalidateWorldMatrix() const
    {
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
    void RenderCamera::notifyChangeWindowResolution_( bool _fullscreen, const Resolution & _resolution )
    {
        MENGINE_UNUSED( _fullscreen );
        MENGINE_UNUSED( _resolution );

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
}
