#include "RenderCameraHelper.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void screenToWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _context, "invalid context" );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->camera, "invalid camera" );

            const RenderCameraInterface * renderCamera = _context->camera;

            mt::vec3f worldPosition;
            renderCamera->fromScreenToWorldPosition( _screenPoint, 0.f, &worldPosition );

            *_worldPoint = worldPosition.to_vec2f();
        }
        //////////////////////////////////////////////////////////////////////////
        void screenToWorldDelta( const RenderContext * _context, const mt::vec2f & _screenDelta, mt::vec2f * const _worldDelta )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _context, "invalid context" );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->camera, "invalid camera" );

            const RenderCameraInterface * renderCamera = _context->camera;

            mt::vec3f worldDelta;
            renderCamera->fromScreenToWorldDelta( _screenDelta, 0.f, &worldDelta );

            *_worldDelta = worldDelta.to_vec2f();
        }
        //////////////////////////////////////////////////////////////////////////
        void worldToScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPosition, mt::vec2f * const _screenPosition )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _context, "invalid context" );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->camera, "invalid camera" );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->viewport, "invalid viewport" );

            const RenderCameraInterface * renderCamera = _context->camera;
            const RenderViewportInterface * renderViewport = _context->viewport;

            const mt::mat4f & vpm = renderCamera->getCameraViewProjectionMatrix();

            mt::vec2f v_screen;
            mt::mul_v2_v2_m4_homogenize( &v_screen, _worldPosition, vpm );

            renderViewport->fromScreenToViewportPosition( v_screen, _screenPosition );
        }
        //////////////////////////////////////////////////////////////////////////
        void worldToScreenDelta( const RenderContext * _context, const mt::vec2f & _worldDelta, mt::vec2f * const _screenDelta )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _context, "invalid context" );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->camera, "invalid camera" );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->viewport, "invalid viewport" );

            const RenderCameraInterface * renderCamera = _context->camera;
            const RenderViewportInterface * renderViewport = _context->viewport;

            const Viewport & vp = renderViewport->getViewportWM();

            mt::vec2f vp_size;
            vp.calcSize( &vp_size );

            const Resolution & contentResolution = renderViewport->getContentResolution();

            mt::vec2f contentResolutionInvSize;
            contentResolution.calcInvSize( &contentResolutionInvSize );

            const mt::mat4f & vpm = renderCamera->getCameraViewProjectionMatrix();

            mt::vec2f v_screen0;
            mt::mul_v2_v2z_m4_homogenize( &v_screen0, vpm );

            mt::vec2f v_screen0n;
            v_screen0n.x = (1.f + v_screen0.x) * 0.5f;
            v_screen0n.y = (1.f - v_screen0.y) * 0.5f;

            mt::vec2f v_screen1;
            mt::mul_v2_v2_m4_homogenize( &v_screen1, _worldDelta, vpm );

            mt::vec2f v_screen1n;
            v_screen1n.x = (1.f + v_screen1.x) * 0.5f;
            v_screen1n.y = (1.f - v_screen1.y) * 0.5f;

            *_screenDelta = (v_screen1n - v_screen0n) * vp_size * contentResolutionInvSize;
        }
        //////////////////////////////////////////////////////////////////////////
        void worldToScreenBox( const RenderContext * _context, const mt::box2f & _worldBox, mt::box2f * const _screenBox )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _context, "invalid context" );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->camera, "invalid camera" );
            MENGINE_ASSERTION_MEMORY_PANIC( _context->viewport, "invalid viewport" );

            const RenderCameraInterface * renderCamera = _context->camera;
            const RenderViewportInterface * renderViewport = _context->viewport;

            const Viewport & vp = renderViewport->getViewportWM();

            mt::vec2f vp_size;
            vp.calcSize( &vp_size );

            const Resolution & contentResolution = renderViewport->getContentResolution();

            mt::vec2f contentResolutionInvSize;
            contentResolution.calcInvSize( &contentResolutionInvSize );

            const mt::mat4f & vpm = renderCamera->getCameraViewProjectionMatrix();

            mt::box2f bb_screen;
            mt::box2_homogenize( &bb_screen, _worldBox.minimum, _worldBox.maximum, vpm );

            mt::box2_scale( &bb_screen, vp_size );
            mt::box2_transpose( &bb_screen, vp.begin );
            mt::box2_scale( &bb_screen, contentResolutionInvSize );

            *_screenBox = bb_screen;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
