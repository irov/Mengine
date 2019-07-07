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
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        class PickerVisitor
        {
        public:
            PickerVisitor( VectorPickerTrapStates & _traps, bool _exclusive )
                : m_traps( _traps )
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
            void visit( const RenderViewportInterfacePtr & _viewport, const RenderCameraInterfacePtr & _camera, const NodePtr & _node )
            {
                RenderInterface * render = _node->getRender();

                if( render != nullptr )
                {
                    RenderViewportInterfacePtr nodeViewport = render->getRenderViewport();

                    if( nodeViewport == nullptr )
                    {
                        nodeViewport = _viewport;
                    }

                    RenderCameraInterfacePtr nodeCamera = render->getRenderCamera();

                    if( nodeCamera == nullptr )
                    {
                        nodeCamera = _camera;
                    }

                    m_currentViewport = nodeViewport;
                    m_currentCamera = nodeCamera;
                }
                else
                {
                    m_currentViewport = _viewport;
                    m_currentCamera = _camera;
                }

                RenderViewportInterfacePtr visitViewport = m_currentViewport;
                RenderCameraInterfacePtr visitCamera = m_currentCamera;

                this->accept( _node );

                const IntrusiveSlugListNodeChild & child = _node->getChildren();

                NodePtr single_child = child.single();

                if( single_child != nullptr )
                {
                    this->visit( visitViewport, visitCamera, single_child );
                }
                else if( child.countSlugs() == 0 )
                {
                    for( IntrusiveSlugListNodeChild::unslug_const_iterator
                        it = child.ubegin(),
                        it_end = child.uend();
                        it != it_end;
                        ++it )
                    {
                        NodePtr children = (*it);

                        this->visit( visitViewport, visitCamera, children );
                    }
                }
                else
                {
                    for( IntrusiveSlugChild it( child ); it.eof() == false; )
                    {
                        NodePtr children( *it );

                        it.next_shuffle();

                        this->visit( visitViewport, visitCamera, children );
                    }
                }
            }

        protected:
            void accept( const NodePtr & _node )
            {
                PickerTrapInterface * trap = _node->getPickerTrap();

                if( trap == nullptr )
                {
                    return;
                }

                PickerTrapState * trapState = trap->propagatePickerTrapState();

                if( trapState == nullptr )
                {
                    return;
                }

                if( m_exclusive == true && trapState->exclusive == false )
                {
                    return;
                }

                if( trapState->dead == true )
                {
                    return;
                }

                PickerTrapStateDesc desc;
                desc.state = trapState;
                desc.viewport = m_currentViewport;
                desc.camera = m_currentCamera;

                m_traps.emplace_back( desc );
            }

        protected:
            VectorPickerTrapStates & m_traps;
            
            bool m_exclusive;

            RenderViewportInterfacePtr m_currentViewport;
            RenderCameraInterfacePtr m_currentCamera;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    PickerService::PickerService()
        : m_pickerTrapCount( 0 )
        , m_pickerTrapExclusiveCount( 0 )
        , m_block( false )
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
        m_scissor = nullptr;

        m_pickerTrapState.clear();
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
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setScene( const ScenePtr & _scene )
    {
        m_scene = _scene;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderViewport( const RenderViewportInterfacePtr & _viewport )
    {
        m_viewport = _viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderCamera( const RenderCameraInterfacePtr & _camera )
    {
        m_camera = _camera;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::setRenderScissor( const RenderScissorInterfacePtr & _scissor )
    {
        m_scissor = _scissor;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::pickTrap( const mt::vec2f& _point, VectorPickerTraps & _traps )
    {
        VectorPickerTrapStates states;
        states.reserve( m_pickerTrapCount );

        if( this->proccesTraps_( _point.x, _point.y, states ) == false )
        {
            return false;
        }

        for( VectorPickerTrapStates::reverse_iterator
            it = states.rbegin(),
            it_end = states.rend();
            it != it_end;
            ++it )
        {
            PickerTrapState * state = it->state;

            if( state->dead == true )
            {
                continue;
            }

            if( state->picked == false )
            {
                continue;
            }

            const PickerTrapInterfacePtr & trap = state->trap;
            _traps.emplace_back( trap );
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

        this->updateDead_();
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::clear()
    {
        this->updateDead_();
    }
    //////////////////////////////////////////////////////////////////////////
    PickerTrapState * PickerService::regTrap( const PickerTrapInterfacePtr & _trap, bool _exclusive )
    {
        PickerTrapState state;

        state.trap = _trap;

        uint32_t id = GENERATE_UNIQUE_IDENTITY();
        state.id = id;
        state.picked = false;
        state.pressed = false;
        state.handle = false;
        state.dead = false;
        state.exclusive = _exclusive;

        if( _exclusive == true )
        {
            ++m_pickerTrapExclusiveCount;
        }
        
        m_pickerTrapState.emplace_back( state );
        PickerTrapState & refState = m_pickerTrapState.back();

        ++m_pickerTrapCount;

        return &refState;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::updateTraps()
    {
        const mt::vec2f & pos = INPUT_SERVICE()
            ->getCursorPosition( 0 );

        VectorPickerTrapStates states;
        states.reserve( m_pickerTrapCount );

        this->proccesTraps_( pos.x, pos.y, states );
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::invalidateTraps()
    {
        m_invalidateTraps = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::unregTrap( PickerTrapState * _ref )
    {
        for( PickerTrapState & state : m_pickerTrapState )
        {
            if( state.id == _ref->id )
            {
                state.dead = true;

                if( state.exclusive == true )
                {
                    --m_pickerTrapExclusiveCount;
                }

                --m_pickerTrapCount;

                if( state.picked == true )
                {
                    state.picked = false;
                    state.trap->onHandleMouseOverDestroy();
                }

                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::handleKeyEvent( const InputKeyEvent & _event )
    {
        m_states.clear();

        if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerTrapStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            PickerTrapState * state = it->state;

            if( state->dead == true )
            {
                continue;
            }

            const PickerTrapInterfacePtr & trap = state->trap;

            if( trap->handleKeyEvent( _event ) == false )
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

        if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerTrapStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            PickerTrapState * state = it->state;

            if( state->dead == true )
            {
                continue;
            }

            const PickerTrapInterfacePtr & trap = state->trap;

            if( trap->handleTextEvent( _event ) == false )
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

        if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerTrapStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerTrapStateDesc & desc = *it;

            PickerTrapState * state = desc.state;

            if( state->dead == true )
            {
                continue;
            }

            if( state->picked == false )
            {
                continue;
            }

            //if( _event.isDown == false && state->pressed == false )
            //{
            //	continue;
            //}

            const PickerTrapInterfacePtr & trap = state->trap;

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            mt::vec2f wp;
            m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), &wp );

            InputMouseButtonEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.isPressed = state->pressed;

            if( trap->handleMouseButtonEvent( ne ) == false )
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

        if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerTrapStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerTrapStateDesc & desc = *it;

            PickerTrapState * state = desc.state;

            if( state->dead == true )
            {
                continue;
            }

            if( state->picked == false )
            {
                continue;
            }

            if( _event.isDown == true )
            {
                state->pressed = true;
            }
            //else if( state->pressed == false )
            //{
            //	continue;
            //}

            const PickerTrapInterfacePtr & trap = state->trap;

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            mt::vec2f wp;
            m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), &wp );

            InputMouseButtonEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.isPressed = state->pressed;

            if( trap->handleMouseButtonEventBegin( ne ) == false )
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

        if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerTrapStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerTrapStateDesc & desc = *it;

            PickerTrapState * state = desc.state;

            if( state->dead == true )
            {
                continue;
            }

            if( state->picked == false )
            {
                continue;
            }

            if( _event.isDown == false )
            {
                if( state->pressed == false )
                {
                    //continue;
                }
                else
                {
                    state->pressed = false;
                }
            }

            const PickerTrapInterfacePtr & trap = state->trap;

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            mt::vec2f wp;
            m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), &wp );

            InputMouseButtonEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.isPressed = state->pressed;

            if( trap->handleMouseButtonEventEnd( ne ) == false )
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

        if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerTrapStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerTrapStateDesc & desc = *it;

            PickerTrapState * state = desc.state;

            if( state->dead == true )
            {
                continue;
            }

            if( state->picked == false )
            {
                continue;
            }

            const PickerTrapInterfacePtr & trap = state->trap;

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            mt::vec2f wp;
            m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), &wp );

            mt::vec2f dp;
            m_arrow->calcPointDeltha( camera, mt::vec2f( _event.dx, _event.dy ), &dp );

            InputMouseMoveEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;
            ne.dx = dp.x;
            ne.dy = dp.y;

            if( trap->handleMouseMove( ne ) == false )
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

        if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
        {
            return false;
        }

        for( VectorPickerTrapStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            const PickerTrapStateDesc & desc = *it;

            PickerTrapState * state = desc.state;

            if( state->dead == true )
            {
                continue;
            }

            if( state->picked == false )
            {
                continue;
            }

            const PickerTrapInterfacePtr & trap = state->trap;

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            mt::vec2f wp;
            m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), &wp );

            InputMouseWheelEvent ne = _event;
            ne.x = wp.x;
            ne.y = wp.y;

            if( trap->handleMouseWheel( ne ) == false )
            {
                continue;
            }

            return m_handleValue;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::handleMouseEnter( const InputMousePositionEvent & _event )
    {
        m_states.clear();

        this->proccesTraps_( _event.x, _event.y, m_states );
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::handleMouseLeave( const InputMousePositionEvent & _event )
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

        PickerVisitor pv( m_states, m_pickerTrapExclusiveCount != 0 );
        pv.visit( m_viewport, m_camera, m_scene );

        for( VectorPickerTrapStates::reverse_iterator
            it = m_states.rbegin(),
            it_end = m_states.rend();
            it != it_end;
            ++it )
        {
            PickerTrapState * state = it->state;

            if( state->dead == true )
            {
                continue;
            }

            const PickerTrapInterfacePtr & trap = state->trap;

            if( state->picked == true )
            {
                state->picked = false;

                trap->onHandleMouseLeave();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PickerService::proccesTraps_( float _x, float _y, VectorPickerTrapStates & _states )
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

        PickerVisitor pv( _states, m_pickerTrapExclusiveCount != 0 );
        pv.visit( m_viewport, m_camera, m_scene );

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f adapt_screen_position;
        m_arrow->adaptScreenPosition_( mt::vec2f( vx, vy ), &adapt_screen_position );

        bool handle = false;

        for( VectorPickerTrapStates::reverse_iterator
            it = _states.rbegin(),
            it_end = _states.rend();
            it != it_end;
            ++it )
        {
            PickerTrapStateDesc & desc = *it;

            PickerTrapState * state = desc.state;

            if( state->dead == true )
            {
                continue;
            }

            const PickerTrapInterfacePtr & trap = state->trap;

            const RenderViewportInterfacePtr & viewport = desc.viewport;
            const RenderCameraInterfacePtr & camera = desc.camera;

            if( handle == false || m_handleValue == false )
            {
                bool picked = trap->pick( adapt_screen_position, viewport, camera, contentResolution, m_arrow );

                if( m_block == false && picked == true )
                {
                    if( state->picked == false )
                    {
                        state->picked = true;

                        handle = trap->onHandleMouseEnter( adapt_screen_position.x, adapt_screen_position.y );

                        state->handle = handle;
                    }
                    else
                    {
                        handle = state->handle;
                    }
                }
                else
                {
                    if( state->picked == true )
                    {
                        state->picked = false;
                        trap->onHandleMouseLeave();
                    }
                }
            }
            else
            {
                if( state->picked == true )
                {
                    state->picked = false;
                    trap->onHandleMouseLeave();
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PickerService::updateDead_()
    {
        for( ListPickerTrapState::iterator
            it = m_pickerTrapState.begin();
            it != m_pickerTrapState.end(); )
        {
            if( it->dead == true )
            {
                it = m_pickerTrapState.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t PickerService::getPickerTrapCount() const
    {
        uint32_t count = 0;

        for( const PickerTrapState & state : m_pickerTrapState )
        {
            if( state.dead == true )
            {
                continue;
            }

            ++count;
        }

        return count;
    }
}
