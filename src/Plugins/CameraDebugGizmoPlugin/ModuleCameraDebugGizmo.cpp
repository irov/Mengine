#include "ModuleCameraDebugGizmo.h"

#include "Interface/PlayerServiceInterface.h"
#include "Interface/InputServiceInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/GlobalInputHandlerHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ModuleCameraDebugGizmo::ModuleCameraDebugGizmo()
        : m_scaleStepMin( 0.f )
        , m_scaleStepStep( 0.f )
        , m_scaleStepMax( 0.f )
        , m_mouseWheelEventHandle( INVALID_UNIQUE_ID )
        , m_mouseMoveEventHandle( INVALID_UNIQUE_ID )
        , m_keyEventHandle( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleCameraDebugGizmo::~ModuleCameraDebugGizmo()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleCameraDebugGizmo::_initializeModule()
    {
        m_scaleStepMin = CONFIG_VALUE( "CameraDebugGizmo", "ScaleStepMin", 0.05f );
        m_scaleStepStep = CONFIG_VALUE( "CameraDebugGizmo", "ScaleStepStep", 0.05f );
        m_scaleStepMax = CONFIG_VALUE( "CameraDebugGizmo", "ScaleStepMax", 128.f );

        m_mouseWheelEventHandle = Helper::addGlobalMouseWheelEvent( [this]( const InputMouseWheelEvent & _event )
        {
            bool controlDown = _event.special.isControl;

            if( controlDown == false )
            {
                return;
            }

            const RenderCameraOrthogonalPtr & camera = PLAYER_SERVICE()
                ->getDefaultSceneRenderCamera2D();

            const mt::vec2f & camera_offset = camera->getCameraOffset();
            const mt::vec2f & camera_scale = camera->getCameraScale();

            const mt::vec2f & pos = INPUT_SERVICE()
                ->getCursorPosition( TC_TOUCH0 );

            mt::vec2f new_camera_offset = camera_offset;
            new_camera_offset.x -= (1.f - pos.x * 2.f) * camera_scale.x;
            new_camera_offset.y -= (1.f - (1.f - pos.y) * 2.f) * camera_scale.y;

            mt::vec2f new_camera_scale = camera_scale;

            float scale_step = float( _event.scroll ) * m_scaleStepStep;

            if( new_camera_scale.x + scale_step > m_scaleStepMax )
            {
                new_camera_scale.x = m_scaleStepMax;
                new_camera_scale.y = m_scaleStepMax;
            }
            else if( new_camera_scale.x + scale_step < m_scaleStepMin )
            {
                new_camera_scale.x = m_scaleStepMin;
                new_camera_scale.y = m_scaleStepMin;
            }
            else
            {
                new_camera_scale.x += scale_step;
                new_camera_scale.y += scale_step;
            }

            new_camera_offset.x += (1.f - pos.x * 2.f) * new_camera_scale.x;
            new_camera_offset.y += (1.f - (1.f - pos.y) * 2.f) * new_camera_scale.y;

            camera->setCameraOffset( new_camera_offset );
            camera->setCameraScale( new_camera_scale );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_mouseMoveEventHandle = Helper::addGlobalMouseMoveEvent( []( const InputMouseMoveEvent & _event )
        {
            bool controlDown = _event.special.isControl;

            bool mouseRButtonDown = INPUT_SERVICE()
                ->isMouseButtonDown( MC_RBUTTON );

            if( controlDown == false || mouseRButtonDown == false )
            {
                return;
            }

            const RenderCameraOrthogonalPtr & camera = PLAYER_SERVICE()
                ->getDefaultSceneRenderCamera2D();

            const mt::vec2f & camera_offset = camera->getCameraOffset();

            mt::vec2f new_camera_offset = camera_offset;
            new_camera_offset.x += _event.dx * 2.f;
            new_camera_offset.y -= _event.dy * 2.f;

            camera->setCameraOffset( new_camera_offset );

        }, MENGINE_DOCUMENT_FACTORABLE );

        m_keyEventHandle = Helper::addGlobalKeyHandler( KC_ESCAPE, true, []( const InputKeyEvent & _event )
        {
            MENGINE_UNUSED( _event );

            const RenderCameraOrthogonalPtr & camera = PLAYER_SERVICE()
                ->getDefaultSceneRenderCamera2D();

            camera->setCameraOffset( mt::vec2f( 0.f, 0.f ) );
            camera->setCameraScale( mt::vec2f( 1.f, 1.f ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleCameraDebugGizmo::_finalizeModule()
    {
        if( m_mouseWheelEventHandle != INVALID_UNIQUE_ID )
        {
            Helper::removeGlobalHandler( m_mouseWheelEventHandle );
            m_mouseWheelEventHandle = INVALID_UNIQUE_ID;
        }

        if( m_mouseMoveEventHandle != INVALID_UNIQUE_ID )
        {
            Helper::removeGlobalHandler( m_mouseMoveEventHandle );
            m_mouseMoveEventHandle = INVALID_UNIQUE_ID;
        }

        if( m_keyEventHandle != INVALID_UNIQUE_ID )
        {
            Helper::removeGlobalHandler( m_keyEventHandle );
            m_keyEventHandle = INVALID_UNIQUE_ID;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
