#include "RenderCameraHelper.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void screenToWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _context );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->camera );

            const RenderCameraInterface * renderCamera = _context->camera;

            const mt::mat4f & pm_inv = renderCamera->getCameraProjectionMatrixInv();

            mt::vec2f p1 = _screenPoint * 2.f - mt::vec2f( 1.f, 1.f );
            p1.y = -p1.y;

            mt::vec2f p_pm;
            mt::mul_v2_v2_m4( p_pm, p1, pm_inv );

            const mt::mat4f & vm = renderCamera->getCameraViewMatrix();

            mt::mat4f vm_inv;
            mt::inv_m4_m4( vm_inv, vm );

            mt::vec2f p = p_pm;

            mt::vec2f p_vm;
            mt::mul_v2_v2_m4( p_vm, p, vm_inv );

            *_worldPoint = p_vm;
        }
        //////////////////////////////////////////////////////////////////////////
        void screenToWorldDelta( const RenderContext * _context, const mt::vec2f & _screenDeltha, mt::vec2f * const _worldDeltha )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _context );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->camera );

            const RenderCameraInterface * renderCamera = _context->camera;

            const mt::mat4f & pm_inv = renderCamera->getCameraProjectionMatrixInv();

            mt::vec2f p3 = (_screenDeltha) * 2.f - mt::vec2f( 1.f, 1.f );
            p3.y = -p3.y;

            mt::vec2f p_pm_deltha;
            mt::mul_v2_v2_m4( p_pm_deltha, p3, pm_inv );

            *_worldDeltha = p_pm_deltha;
        }
        //////////////////////////////////////////////////////////////////////////
        void worldToScreenPosition( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _worldPosition, mt::vec2f * const _screenPosition )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _context );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->camera );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->viewport );

            const RenderCameraInterface * renderCamera = _context->camera;
            const RenderViewportInterface * renderViewport = _context->viewport;

            const Viewport & vp = renderViewport->getViewport();

            mt::vec2f vp_size;
            vp.calcSize( vp_size );

            mt::vec2f contentResolutionInvSize;
            _contentResolution.calcInvSize( &contentResolutionInvSize );

            const mt::mat4f & vpm = renderCamera->getCameraViewProjectionMatrix();

            mt::vec2f v_screen;
            mt::mul_v2_v2_m4_homogenize( v_screen, _worldPosition, vpm );

            *_screenPosition = (vp.begin + v_screen * vp_size) * contentResolutionInvSize;
        }
        //////////////////////////////////////////////////////////////////////////
        void worldToScreenDelta( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _worldDelta, mt::vec2f * const _screenDelta )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _context );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->camera );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->viewport );

            const RenderCameraInterface * renderCamera = _context->camera;
            const RenderViewportInterface * renderViewport = _context->viewport;

            const Viewport & vp = renderViewport->getViewport();

            mt::vec2f vp_size;
            vp.calcSize( vp_size );

            mt::vec2f contentResolutionInvSize;
            _contentResolution.calcInvSize( &contentResolutionInvSize );

            const mt::mat4f & vpm = renderCamera->getCameraViewProjectionMatrix();

            mt::vec2f v_screen0;
            mt::mul_v2_v2z_m4_homogenize( v_screen0, vpm );

            v_screen0.x += 1.f;
            v_screen0.y = 1.f - v_screen0.y;

            v_screen0 *= mt::vec2f( 0.5f, 0.5f );

            mt::vec2f v_screen;
            mt::mul_v2_v2_m4_homogenize( v_screen, _worldDelta, vpm );

            v_screen.x += 1.f;
            v_screen.y = 1.f - v_screen.y;

            v_screen *= mt::vec2f( 0.5f, 0.5f );

            *_screenDelta = (v_screen - v_screen0) * vp_size * contentResolutionInvSize;
        }
        //////////////////////////////////////////////////////////////////////////
        void worldToScreenBox( const RenderContext * _context, const Resolution & _contentResolution, const mt::box2f & _worldBox, mt::box2f * const _screenBox )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _context );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->camera );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->viewport );

            const RenderCameraInterface * renderCamera = _context->camera;
            const RenderViewportInterface * renderViewport = _context->viewport;

            const Viewport & vp = renderViewport->getViewport();

            mt::vec2f vp_size;
            vp.calcSize( vp_size );

            mt::vec2f contentResolutionInvSize;
            _contentResolution.calcInvSize( &contentResolutionInvSize );

            const mt::mat4f & vpm = renderCamera->getCameraViewProjectionMatrix();

            mt::box2f bb_screen;
            mt::set_box_homogenize( bb_screen, _worldBox.minimum, _worldBox.maximum, vpm );

            mt::scale_box( bb_screen, vp_size );
            mt::transpose_box( bb_screen, vp.begin );
            mt::scale_box( bb_screen, contentResolutionInvSize );

            *_screenBox = bb_screen;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
