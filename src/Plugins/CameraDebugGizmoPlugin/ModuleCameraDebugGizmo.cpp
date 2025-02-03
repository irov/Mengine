#include "ModuleCameraDebugGizmo.h"

#include "Interface/PlayerServiceInterface.h"
#include "Interface/InputServiceInterface.h"

#include "Kernel/ConfigHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ModuleCameraDebugGizmo::ModuleCameraDebugGizmo()
        : m_scaleStepMin( 0.f )
        , m_scaleStepStep( 0.f )
        , m_scaleStepMax( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleCameraDebugGizmo::~ModuleCameraDebugGizmo()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleCameraDebugGizmo::_initializeModule()
    {
        m_scaleStepMin = CONFIG_VALUE_FLOAT( "CameraDebugGizmoPlugin", "ScaleStepMin", 0.05f );
        m_scaleStepStep = CONFIG_VALUE_FLOAT( "CameraDebugGizmoPlugin", "ScaleStepStep", 0.05f );
        m_scaleStepMax = CONFIG_VALUE_FLOAT( "CameraDebugGizmoPlugin", "ScaleStepMax", 128.f );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleCameraDebugGizmo::_finalizeModule()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleCameraDebugGizmo::_handleKeyEvent( const InputKeyEvent & _event )
    {
        if( _event.code == KC_ESCAPE && _event.isDown == true )
        {
            const RenderCameraOrthogonalPtr & camera = PLAYER_SERVICE()
                ->getDefaultSceneRenderCamera2D();

            camera->setCameraOffset( mt::vec2f( 0.f, 0.f ) );
            camera->setCameraScale( mt::vec2f( 1.f, 1.f ) );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleCameraDebugGizmo::_handleMouseMove( const InputMouseMoveEvent & _event )
    {
        bool controlDown = _event.special.isControl;

        bool mouseRButtonDown = INPUT_SERVICE()
            ->isMouseButtonDown( MC_RBUTTON );

        if( controlDown == false || mouseRButtonDown == false )
        {
            return false;
        }

        const RenderCameraOrthogonalPtr & camera = PLAYER_SERVICE()
            ->getDefaultSceneRenderCamera2D();

        const mt::vec2f & camera_offset = camera->getCameraOffset();

        mt::vec2f new_camera_offset = camera_offset;
        new_camera_offset.x += _event.dx * 2.f;
        new_camera_offset.y -= _event.dy * 2.f;

        camera->setCameraOffset( new_camera_offset );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleCameraDebugGizmo::_handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        bool controlDown = _event.special.isControl;

        if( controlDown == false )
        {
            return false;
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

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
