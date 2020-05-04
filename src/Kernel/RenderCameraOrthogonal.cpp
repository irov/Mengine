#include "RenderCameraOrthogonal.h"

#include "Interface/ApplicationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderCameraOrthogonal::RenderCameraOrthogonal()
        : m_cameraPosition( 0.f, 0.f, 0.f )
        , m_cameraDirection( 0.f, 0.f, 1.f )
        , m_cameraUp( 0.f, 1.f, 0.f )
        , m_cameraRightSign( 1.f )
        , m_cameraNear( -10000.f )
        , m_cameraFar( 10000.f )
        , m_cameraScale( 1.f, 1.f )
        , m_cameraOffset( 0.f, 0.f )
        , m_proxyViewMatrix( false )
        , m_fixedOrthogonalViewport( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderCameraOrthogonal::~RenderCameraOrthogonal()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::fromWorldToScreenPosition( const mt::mat4f & _worldMatrix, mt::vec2f & _screenPosition ) const
    {
        const mt::mat4f & vpm = this->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( wvpm, _worldMatrix, vpm );

        mt::vec2f v_clip;
        mt::mul_v2_v2z_m4_homogenize( v_clip, wvpm );

        mt::vec2f v_wvpn;
        v_wvpn.x = (1.f + v_clip.x) * 0.5f;
        v_wvpn.y = (1.f - v_clip.y) * 0.5f;

        Viewport vp;
        this->makeViewport_( vp );

        mt::vec2f vpSize = (vp.end - vp.begin);

        _screenPosition = vp.begin + vpSize * v_wvpn;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraPosition( const mt::vec3f & _pos )
    {
        m_cameraPosition = _pos;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraDirection( const mt::vec3f & _dir )
    {
        m_cameraDirection = _dir;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraUp( const mt::vec3f & _up )
    {
        m_cameraUp = _up;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraRightSign( float _rightSign )
    {
        m_cameraRightSign = _rightSign;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraNear( float _near )
    {
        m_cameraNear = _near;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraFar( float _far )
    {
        m_cameraFar = _far;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraScale( const mt::vec2f & _scale )
    {
        m_cameraScale = _scale;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & RenderCameraOrthogonal::getCameraScale() const
    {
        return m_cameraScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraOffset( const mt::vec2f & _offset )
    {
        m_cameraOffset = _offset;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & RenderCameraOrthogonal::getCameraOffset() const
    {
        return m_cameraOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setOrthogonalViewport( const Viewport & _viewport )
    {
        m_orthogonalViewport = _viewport;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderCameraOrthogonal::getOrthogonalViewport() const
    {
        return m_orthogonalViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setFixedOrthogonalViewport( bool _value )
    {
        m_fixedOrthogonalViewport = _value;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderCameraOrthogonal::getFixedOrthogonalViewport() const
    {
        return m_fixedOrthogonalViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setProxyViewMatrix( bool _value )
    {
        m_proxyViewMatrix = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderCameraOrthogonal::getProxyViewMatrix() const
    {
        return m_proxyViewMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::_updateViewMatrix() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        if( m_proxyViewMatrix == false )
        {
            mt::vec3f wm_position;
            mt::mul_v3_v3_m4( wm_position, m_cameraPosition, wm );

            mt::vec3f wm_direction;
            mt::mul_v3_v3_m4_r( wm_direction, m_cameraDirection, wm );

            mt::vec3f wm_up;
            mt::mul_v3_v3_m4_r( wm_up, m_cameraUp, wm );

            mt::make_lookat_m4( m_viewMatrix, wm_position, wm_direction, wm_up, m_cameraRightSign );

            mt::inv_m4_m4( m_viewMatrixInv, m_viewMatrix );
        }
        else
        {
            mt::inv_m4_m4( m_viewMatrix, wm );

            m_viewMatrixInv = wm;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::_updateProjectionMatrix() const
    {
        Viewport renderViewport;
        this->makeViewport_( renderViewport );

        mt::vec2f rvbegin = renderViewport.begin;
        mt::vec2f rvend = renderViewport.end;

        mt::mat4f pm;
        mt::make_projection_ortho_lh_m4( pm, rvbegin.x, rvend.x, rvbegin.y, rvend.y, m_cameraNear, m_cameraFar );

        mt::mat4f pmscale;
        mt::make_scale_m4( pmscale, m_cameraScale.x, m_cameraScale.y, 1.f );

        mt::mat4f pmoffset;
        mt::make_translation_m4( pmoffset, m_cameraOffset.x, m_cameraOffset.y, 0.f );

        mt::mat4f pm0;
        mt::mul_m4_m4( pm0, pm, pmscale );
        mt::mul_m4_m4( m_projectionMatrix, pm0, pmoffset );

        mt::inv_m4_m4( m_projectionMatrixInv, m_projectionMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::makeViewport_( Viewport & _viewport ) const
    {
        if( m_fixedOrthogonalViewport == false )
        {
            float gameViewportAspect;
            Viewport gameViewport;

            APPLICATION_SERVICE()
                ->getGameViewport( gameViewportAspect, gameViewport );

            Viewport renderViewportWM;

            const mt::mat4f & wm = this->getWorldMatrix();

            mt::mul_v2_v2_m4( renderViewportWM.begin, m_orthogonalViewport.begin, wm );
            mt::mul_v2_v2_m4( renderViewportWM.end, m_orthogonalViewport.end, wm );

            renderViewportWM.clamp( gameViewport );

            mt::mat4f wm_inv;
            mt::inv_m4_m4( wm_inv, wm );

            mt::mul_v2_v2_m4( _viewport.begin, renderViewportWM.begin, wm_inv );
            mt::mul_v2_v2_m4( _viewport.end, renderViewportWM.end, wm_inv );
        }
        else
        {
            _viewport = m_orthogonalViewport;
        }
    }
}
