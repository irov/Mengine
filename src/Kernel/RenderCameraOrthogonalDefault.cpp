#include "RenderCameraOrthogonalDefault.h"

#include "Interface/ApplicationInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderCameraOrthogonalDefault::RenderCameraOrthogonalDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderCameraOrthogonalDefault::~RenderCameraOrthogonalDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonalDefault::fromWorldToScreenPosition( const mt::mat4f & _worldMatrix, mt::vec2f * const _screenPosition ) const
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
    void RenderCameraOrthogonalDefault::fromScreenToWorldPosition( const mt::vec2f & _screenPoint, float _deep, mt::vec3f * const _worldPosition ) const
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
    void RenderCameraOrthogonalDefault::fromScreenToWorldDelta( const mt::vec2f & _screenDelta, float _deep, mt::vec3f * const _worldDelta ) const
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
    void RenderCameraOrthogonalDefault::_updateViewMatrix() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::inv_m4_m4( &m_viewMatrix, wm );
        m_viewMatrixInv = wm;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonalDefault::_updateProjectionMatrix() const
    {
        Viewport renderViewport;
        this->makeViewport_( &renderViewport );

        const mt::vec2f & rvbegin = renderViewport.begin;
        const mt::vec2f & rvend = renderViewport.end;

        mt::make_projection_ortho_lh_m4( &m_projectionMatrix, rvbegin.x, rvend.x, rvbegin.y, rvend.y, -10000.f, 10000.f );

        mt::inv_m4_m4( &m_projectionMatrixInv, m_projectionMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraOrthogonalDefault::makeViewport_( Viewport * const _viewport ) const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        float gameViewportAspect;
        Viewport gameViewport;
        APPLICATION_SERVICE()
            ->getGameViewport( &gameViewportAspect, &gameViewport );

        Viewport renderViewportWM;
        gameViewport.multiply( &renderViewportWM, wm );

        renderViewportWM.clamp( gameViewport );

        mt::mat4f wm_inv;
        mt::inv_m4_m4( &wm_inv, wm );

        mt::mul_v2_v2_m4( &_viewport->begin, renderViewportWM.begin, wm_inv );
        mt::mul_v2_v2_m4( &_viewport->end, renderViewportWM.end, wm_inv );
    }
    //////////////////////////////////////////////////////////////////////////
}
