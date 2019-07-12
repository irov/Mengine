#include "PickerService.h"

#include "Interface/InputServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Arrow.h"
#include "Kernel/Scene.h"

#include "Kernel/Logger.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PickerService, Mengine::PickerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PickerService::PickerService()
        : m_block( false )
        , m_handleValue( true )
        , m_invalidateTraps( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PickerService::~PickerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::_finalizeService()
    {
        m_arrow = nullptr;
        m_scene = nullptr;

        m_viewport = nullptr;
        m_camera = nullptr;

        m_states.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setBlock( bool _value )
    {
        m_block = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setHandleValue( bool _value )
    {
        m_handleValue = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setArrow( const ArrowPtr & _arrow )
    {
        m_arrow = _arrow;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setScene( const ScenePtr & _scene )
    {
        m_scene = _scene;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderViewport( const RenderViewportInterfacePtr & _viewport )
    {
        m_viewport = _viewport;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderCamera( const RenderCameraInterfacePtr & _camera )
    {
        m_camera = _camera;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderScissor( const RenderScissorInterfacePtr & _scissor )
    {
        m_scissor = _scissor;

        this->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::pickTrap( const mt::vec2f & _point, VectorPickers & _pickers )
    {
        VectorPickerStates states;
        if( this->proccesStates_( _point.x, _point.y, states ) == false )
        {
            return false;
        }

        for( VectorPickerStates::reverse_iterator
            it = states.rbegin(),
            it_end = states.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            _pickers.emplace_back( picker );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::update()
    {
        if( m_invalidateTraps == true )
        {
            this->updateTraps();

            m_invalidateTraps = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::clear()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::updateTraps()
    {
        const mt::vec2f & pos = INPUT_SERVICE()
            ->getCursorPosition( 0 );

        VectorPickerStates states;
        this->proccesStates_( pos.x, pos.y, states );
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::invalidateTraps()
    {
        m_invalidateTraps = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleKeyEvent( const InputKeyEvent & _event )
    {
        m_states.clear();

        if( this->proccesStates_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            if( inputHandler->handleKeyEvent( _event ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleTextEvent( const InputTextEvent & _event )
    {
        m_states.clear();

        if( this->proccesStates_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            if( inputHandler->handleTextEvent( _event ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        m_states.clear();

        if( this->proccesStates_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            //if( _event.isDown == false && state->pressed == false )
            //{
            //	continue;
            //}

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            mt::vec2f wp;
            m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), &wp );

            InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseButtonEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.isPressed = picker->isPickerPressed();

            if( inputHandler->handleMouseButtonEvent( ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        m_states.clear();

        if( this->proccesStates_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            if( _event.isDown == true )
            {
                picker->setPickerPressed( true );
            }
            //else if( state->pressed == false )
            //{
            //	continue;
            //}

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            mt::vec2f wp;
            m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), &wp );

            InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseButtonEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.isPressed = picker->isPickerPressed();

            if( inputHandler->handleMouseButtonEventBegin( ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        m_states.clear();

        if( this->proccesStates_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            if( _event.isDown == false )
            {
                if( picker->isPickerPressed() == false )
                {
                    //continue;
                }
                else
                {
                    picker->setPickerPressed( false );
                }
            }

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            mt::vec2f wp;
            m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), &wp );

            InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseButtonEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.isPressed = picker->isPickerPressed();

            if( inputHandler->handleMouseButtonEventEnd( ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        m_states.clear();

        if( this->proccesStates_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            mt::vec2f wp;
            m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), &wp );

            mt::vec2f dp;
            m_arrow->calcPointDeltha( camera, mt::vec2f( _event.dx, _event.dy ), &dp );

            InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseMoveEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.dx = dp.x;
            ne.dy = dp.y;

            if( inputHandler->handleMouseMove( ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        m_states.clear();

        if( this->proccesStates_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            mt::vec2f wp;
            m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), &wp );

            InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseWheelEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;

            if( inputHandler->handleMouseWheel( ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        m_states.clear();

        this->proccesStates_( _event.x, _event.y, m_states );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _event );

        if( m_arrow == nullptr )
        {
            return;
        }

        if( m_scene == nullptr )
        {
            return;
        }

        m_states.clear();
        this->fillStates_( m_states );

        for( VectorPickerStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            if( picker->isPickerPicked() == true )
            {
                picker->setPickerPicked( false );

                InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

                InputMouseLeaveEvent el = _event;

                inputHandler->handleMouseLeave( el );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::fillStates_( VectorPickerStates & _states ) const
    {
        PickerStateDesc root_desc;

        PickerInterface * root_picker = m_scene->getPicker();

        const RenderViewportInterfacePtr & pickerViewport = root_picker->getPickerViewport();

        if( pickerViewport != nullptr )
        {
            root_desc.viewport = pickerViewport;
        }
        else
        {
            root_desc.viewport = m_viewport;
        }

        const RenderCameraInterfacePtr & pickerCamera = root_picker->getPickerCamera();

        if( pickerCamera != nullptr )
        {
            root_desc.camera = pickerCamera;
        }
        else
        {
            root_desc.camera = m_camera;
        }

        _states.push_back( root_desc );

        for( VectorPickerStates::size_type index = 0; index != _states.size(); ++index )
        {
            PickerStateDesc & desc = _states[index];

            PickerInterface * picker = desc.picker;
            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            picker->foreachPickerChildren( [&_states, &viewport, &camera]( PickerInterface * _picker )
            {
                PickerStateDesc child_desc;

                child_desc.picker = _picker;

                const RenderViewportInterfacePtr & pickerViewport = _picker->getPickerViewport();

                if( pickerViewport != nullptr )
                {
                    child_desc.viewport = pickerViewport;
                }
                else
                {
                    child_desc.viewport = viewport;
                }

                const RenderCameraInterfacePtr & pickerCamera = _picker->getPickerCamera();

                if( pickerCamera != nullptr )
                {
                    child_desc.camera = pickerCamera;
                }
                else
                {
                    child_desc.camera = camera;
                }

                _states.push_back( child_desc );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::proccesStates_( float _x, float _y, VectorPickerStates & _states )
    {
        if( m_arrow == nullptr )
        {
            return false;
        }

        if( m_scene == nullptr )
        {
            return false;
        }

        if( m_camera == nullptr )
        {
            return false;
        }

        float vx;
        float vy;
        if( INPUT_SERVICE()
            ->validCursorPosition( _x, _y, &vx, &vy ) == false )
        {
            return false;
        }

        this->fillStates_( _states );

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f adapt_screen_position;
        m_arrow->adaptScreenPosition_( mt::vec2f( vx, vy ), &adapt_screen_position );

        bool handle = false;

        for( VectorPickerStates::reverse_iterator
            it = _states.rbegin(),
            it_end = _states.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            PickerInterface * picker = desc.picker;

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            if( handle == false || m_handleValue == false )
            {
                bool picked = picker->pick( adapt_screen_position, viewport, camera, contentResolution, m_arrow );

                if( m_block == false && picked == true )
                {
                    if( picker->isPickerPicked() == false )
                    {
                        picker->setPickerPicked( true );

                        InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

                        InputMouseEnterEvent ne;
                        ne.type = IET_MOUSE_ENTER;
                        ne.touchId = 0;
                        ne.x = adapt_screen_position.x;
                        ne.y = adapt_screen_position.y;
                        ne.pressure = 0.f;

                        handle = inputHandler->handleMouseEnter( ne );

                        picker->setPickerHandle( handle );
                    }
                    else
                    {
                        handle = picker->isPickerHandle();
                    }
                }
                else
                {
                    if( picker->isPickerPicked() == true )
                    {
                        picker->setPickerPicked( false );

                        InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

                        InputMouseLeaveEvent ne;
                        ne.type = IET_MOUSE_LEAVE;
                        ne.touchId = 0;
                        ne.x = adapt_screen_position.x;
                        ne.y = adapt_screen_position.y;
                        ne.pressure = 0.f;

                        inputHandler->handleMouseLeave( ne );
                    }
                }
            }
            else
            {
                if( picker->isPickerPicked() == true )
                {
                    picker->setPickerPicked( false );

                    InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

                    InputMouseLeaveEvent ne;
                    ne.type = IET_MOUSE_LEAVE;
                    ne.touchId = 0;
                    ne.x = adapt_screen_position.x;
                    ne.y = adapt_screen_position.y;
                    ne.pressure = 0.f;

                    inputHandler->handleMouseLeave( ne );
                }
            }
        }

        return true;
    }
}
