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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PickerVisitor
        {
        public:
            PickerVisitor( VectorPickerStates & _states, bool _exclusive )
                : m_states( _states )
                , m_exclusive( _exclusive )
            {
            }

            ~PickerVisitor()
            {
            }

        protected:
            void operator = ( const PickerVisitor & )
            {
            }

        public:
            void visit( PickerInterface * _picker, const RenderViewportInterfacePtr & _viewport, const RenderCameraInterfacePtr & _camera )
            {
                PickerStateDesc desc;

                if( _picker->isPickerEnable() == false )
                {
                    return;
                }

                if( m_exclusive == true && _picker->isPickerExclusive() == false )
                {
                    return;
                }

                desc.picker = _picker;

                const RenderViewportInterfacePtr & pickerViewport = _picker->getPickerViewport();

                if( pickerViewport != nullptr )
                {
                    desc.viewport = pickerViewport;
                }
                else
                {
                    desc.viewport = _viewport;
                }

                const RenderCameraInterfacePtr & pickerCamera = _picker->getPickerCamera();

                if( pickerCamera != nullptr )
                {
                    desc.camera = pickerCamera;
                }
                else
                {
                    desc.camera = _camera;
                }

                m_states.push_back( desc );

                RenderViewportInterfacePtr visitViewport = desc.viewport;
                RenderCameraInterfacePtr visitCamera = desc.camera;

                _picker->foreachPickerChildrenEnabled( [this, &visitViewport, &visitCamera]( PickerInterface * _picker )
                {
                    this->visit( _picker, visitViewport, visitCamera );
                } );
            }

        protected:
            VectorPickerStates & m_states;

            bool m_exclusive;
        };
    }
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
    bool PickerService::pickTrap( const mt::vec2f & _point, uint32_t _touchId, float _pressure, VectorPickers & _pickers )
    {
        //m_statesAux.clear();
        VectorPickerStates statesAux;
        if( this->proccesStates_( _point.x, _point.y, _touchId, _pressure, statesAux ) == false )
        {
            return false;
        }

        for( VectorPickerStates::reverse_iterator
            it = statesAux.rbegin(),
            it_end = statesAux.rend();
            it != it_end;
            ++it )
        {
            PickerStateDesc & desc = *it;

            const PickerInterfacePtr & picker = desc.picker;

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
        const mt::vec2f & position = INPUT_SERVICE()
            ->getCursorPosition( 0 );

        float pressure = INPUT_SERVICE()
            ->getCursorPressure( 0 );

        VectorPickerStates statesAux;
        this->proccesStates_( position.x, position.y, 0, pressure, statesAux );
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

        if( this->proccesStates_( _event.x, _event.y, 0, 0.f, m_states ) == false )
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

            const PickerInterfacePtr & picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

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

        if( this->proccesStates_( _event.x, _event.y, 0, 0.f, m_states ) == false )
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

            const PickerInterfacePtr & picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

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

        if( this->proccesStates_( _event.x, _event.y, _event.touchId, _event.pressure, m_states ) == false )
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

            const PickerInterfacePtr & picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

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

        if( this->proccesStates_( _event.x, _event.y, _event.touchId, _event.pressure, m_states ) == false )
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

            const PickerInterfacePtr & picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

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

        if( this->proccesStates_( _event.x, _event.y, _event.touchId, _event.pressure, m_states ) == false )
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

            const PickerInterfacePtr & picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

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

        if( this->proccesStates_( _event.x, _event.y, _event.touchId, _event.pressure, m_states ) == false )
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

            const PickerInterfacePtr & picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

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

        if( this->proccesStates_( _event.x, _event.y, 0, 0.f, m_states ) == false )
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

            const PickerInterfacePtr & picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

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

        this->proccesStates_( _event.x, _event.y, _event.touchId, _event.pressure, m_states );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
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

            const PickerInterfacePtr & picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

            if( picker->isPickerPicked() == false )
            {
                continue;
            }

            picker->setPickerPicked( false );

            InputHandlerInterface * inputHandler = picker->getPickerInputHandler();

            InputMouseLeaveEvent el = _event;

            inputHandler->handleMouseLeave( el );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::fillStates_( VectorPickerStates & _states ) const
    {
        PickerInterface * picker = m_scene->getPicker();

        Detail::PickerVisitor visitor( _states, false );
        visitor.visit( picker, m_viewport, m_camera );

        VectorPickerStates::iterator it_erase = std::remove_if( _states.begin(), _states.end(), []( const PickerStateDesc & _desc )
        {
            return _desc.picker->isPickerDummy();
        } );
        _states.erase( it_erase, _states.end() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::proccesStates_( float _x, float _y, uint32_t _touchId, float _pressure, VectorPickerStates & _states )
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
            const PickerStateDesc & desc = *it;

            const PickerInterfacePtr & picker = desc.picker;

            if( picker->isPickerEnable() == false )
            {
                continue;
            }

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
                        ne.touchId = _touchId;
                        ne.x = adapt_screen_position.x;
                        ne.y = adapt_screen_position.y;
                        ne.pressure = _pressure;

                        handle = inputHandler->handleMouseEnter( ne );

                        picker->setPickerHandle( handle );
                    }
                    else
                    {
                        bool pickerHandle = picker->isPickerHandle();

                        handle = pickerHandle;
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
                        ne.touchId = _touchId;
                        ne.x = adapt_screen_position.x;
                        ne.y = adapt_screen_position.y;
                        ne.pressure = _pressure;

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
                    ne.touchId = _touchId;
                    ne.x = adapt_screen_position.x;
                    ne.y = adapt_screen_position.y;
                    ne.pressure = _pressure;

                    inputHandler->handleMouseLeave( ne );
                }
            }
        }

        return true;
    }
}
