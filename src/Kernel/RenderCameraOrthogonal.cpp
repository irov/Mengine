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
        , m_clampViewport( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderCameraOrthogonal::~RenderCameraOrthogonal()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::fromWorldToScreenPosition( const mt::mat4f & _worldMatrix, mt::vec2f * const _screenPosition ) const
    {
        const mt::mat4f & vpm = this->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( &wvpm, _worldMatrix, vpm );

        mt::vec2f v_clip;
        mt::mul_v2_v2z_m4_homogenize( &v_clip, wvpm );

        mt::vec2f v_wvpn;
        v_wvpn.x = (1.f + v_clip.x) * 0.5f;
        v_wvpn.y = (1.f - v_clip.y) * 0.5f;

        *_screenPosition = v_wvpn;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::fromScreenToWorldPosition( const mt::vec2f & _screenPoint, float _deep, mt::vec3f * const _worldPosition ) const
    {
        const mt::mat4f & pm_inv = this->getCameraProjectionMatrixInv();

        mt::vec2f p1 = _screenPoint * 2.f - mt::vec2f( 1.f, 1.f );
        p1.y = -p1.y;

        mt::vec2f p_pm;
        mt::mul_v2_v2_m4( &p_pm, p1, pm_inv );

        const mt::mat4f & vm = this->getCameraViewMatrix();

        mt::mat4f vm_inv;
        mt::inv_m4_m4( &vm_inv, vm );

        mt::vec2f p = p_pm;

        mt::vec3f p_vm;
        mt::mul_v3_v2_m4( &p_vm, p, vm_inv );

        //Viewport vp;
        //this->makeViewport_( &vp );

        //mt::vec3f wp;
        //wp.x = p_vm.x - vp.begin.x;
        //wp.y = p_vm.y - vp.begin.y;
        //wp.z = p_vm.z + _deep;

        mt::vec3f wp;
        wp.x = p_vm.x;
        wp.y = p_vm.y;
        wp.z = p_vm.z + _deep;

        *_worldPosition = wp;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::fromScreenToWorldDelta( const mt::vec2f & _screenDelta, float _deep, mt::vec3f * const _worldDelta ) const
    {
        MENGINE_UNUSED( _deep );
        //TODO: implement deep

        const mt::mat4f & pm_inv = this->getCameraViewProjectionMatrixInv();

        mt::vec3f p_pm_base;
        mt::mul_v3_v2z_m4( &p_pm_base, pm_inv );

        mt::vec2f p_delta = _screenDelta * 2.f;
        p_delta.y = -p_delta.y;

        mt::vec3f p_pm_delta;
        mt::mul_v3_v2_m4( &p_pm_delta, p_delta, pm_inv );

        mt::vec3f wd;
        wd.x = p_pm_delta.x - p_pm_base.x;
        wd.y = p_pm_delta.y - p_pm_base.y;
        wd.z = 0.f;

        *_worldDelta = wd;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraPosition( const mt::vec3f & _cameraPosition )
    {
        m_cameraPosition = _cameraPosition;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraDirection( const mt::vec3f & _cameraDirection )
    {
        m_cameraDirection = _cameraDirection;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraUp( const mt::vec3f & _cameraUp )
    {
        m_cameraUp = _cameraUp;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraRightSign( float _cameraRightSign )
    {
        m_cameraRightSign = _cameraRightSign;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraNear( float _cameraNear )
    {
        m_cameraNear = _cameraNear;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraFar( float _cameraFar )
    {
        m_cameraFar = _cameraFar;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraScale( const mt::vec2f & _cameraScale )
    {
        m_cameraScale = _cameraScale;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & RenderCameraOrthogonal::getCameraScale() const
    {
        return m_cameraScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setCameraOffset( const mt::vec2f & _cameraOffset )
    {
        m_cameraOffset = _cameraOffset;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & RenderCameraOrthogonal::getCameraOffset() const
    {
        return m_cameraOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setOrthogonalViewport( const Viewport & _orthogonalViewport )
    {
        m_orthogonalViewport = _orthogonalViewport;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderCameraOrthogonal::getOrthogonalViewport() const
    {
        return m_orthogonalViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setFixedOrthogonalViewport( bool _fixedOrthogonalViewport )
    {
        m_fixedOrthogonalViewport = _fixedOrthogonalViewport;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderCameraOrthogonal::getFixedOrthogonalViewport() const
    {
        return m_fixedOrthogonalViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setProxyViewMatrix( bool _proxyViewMatrix )
    {
        m_proxyViewMatrix = _proxyViewMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderCameraOrthogonal::getProxyViewMatrix() const
    {
        return m_proxyViewMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::setClampViewport( bool _clampViewport )
    {
        m_clampViewport = _clampViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderCameraOrthogonal::getClampViewport() const
    {
        return m_clampViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::_updateViewMatrix() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        if( m_proxyViewMatrix == false )
        {
            mt::mat4f lm;
            mt::make_lookat_m4( &lm, m_cameraPosition, m_cameraDirection, m_cameraUp, m_cameraRightSign );

            mt::mat4f wm_inv;
            mt::inv_m4_m4( &wm_inv, wm );

            mt::mul_m4_m4( &m_viewMatrix, lm, wm_inv );
            mt::inv_m4_m4( &m_viewMatrixInv, m_viewMatrix );
        }
        else
        {
            mt::inv_m4_m4( &m_viewMatrix, wm );

            m_viewMatrixInv = wm;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::_updateProjectionMatrix() const
    {
        Viewport renderViewport;
        this->makeViewport_( &renderViewport );

        const mt::vec2f & rvbegin = renderViewport.begin;
        const mt::vec2f & rvend = renderViewport.end;

        mt::mat4f pm;
        mt::make_projection_ortho_lh_m4( &pm, rvbegin.x, rvend.x, rvbegin.y, rvend.y, m_cameraNear, m_cameraFar );

        mt::mat4f pmscale;
        mt::make_scale_m4( &pmscale, m_cameraScale.x, m_cameraScale.y, 1.f );

        mt::mat4f pmoffset;
        mt::make_translation_m4( &pmoffset, m_cameraOffset.x, m_cameraOffset.y, 0.f );

        mt::mat4f pm0;
        mt::mul_m4_m4( &pm0, pm, pmscale );
        mt::mul_m4_m4( &m_projectionMatrix, pm0, pmoffset );

        mt::inv_m4_m4( &m_projectionMatrixInv, m_projectionMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonal::makeViewport_( Viewport * const _viewport ) const
    {
        if( m_fixedOrthogonalViewport == false )
        {
            const mt::mat4f & wm = this->getWorldMatrix();

            Viewport renderViewportWM;
            m_orthogonalViewport.multiply( &renderViewportWM, wm );

            if( m_clampViewport == true )
            {
                float gameViewportAspect;
                Viewport gameViewport;
                APPLICATION_SERVICE()
                    ->getGameViewport( &gameViewportAspect, &gameViewport );

                renderViewportWM.clamp( gameViewport );
            }

            mt::mat4f wm_inv;
            mt::inv_m4_m4( &wm_inv, wm );

            mt::mul_v2_v2_m4( &_viewport->begin, renderViewportWM.begin, wm_inv );
            mt::mul_v2_v2_m4( &_viewport->end, renderViewportWM.end, wm_inv );
        }
        else
        {
            *_viewport = m_orthogonalViewport;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
