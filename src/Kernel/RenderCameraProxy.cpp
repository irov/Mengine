#include "RenderCameraProxy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderCameraProxy::RenderCameraProxy()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderCameraProxy::~RenderCameraProxy()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProxy::initialize( const mt::mat4f & _pm, const mt::mat4f & _vm )
    {
        m_projectionMatrix = _pm;
        mt::inv_m4_m4( &m_projectionMatrixInv, m_projectionMatrix );

        m_viewMatrix = _vm;
        mt::inv_m4_m4( &m_viewMatrixInv, m_viewMatrix );

        mt::mul_m4_m4( &m_viewProjectionMatrix, m_viewMatrix, m_projectionMatrix );
        mt::inv_m4_m4( &m_viewProjectionMatrixInv, m_viewProjectionMatrix );
    }
}
