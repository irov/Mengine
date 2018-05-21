#include "MousePickerSystem.h"

#include "Interface/InputSystemInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Kernel/Arrow.h"
#include "Kernel/Scene.h"

#include "Logger/Logger.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MousePickerSystem, Mengine::MousePickerSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		class PickerVisitor
		{
		public:
			PickerVisitor( TVectorPickerTrapStates & _traps )
				: m_traps(_traps)
				, m_currentViewport(nullptr)
				, m_currentCamera(nullptr)				
			{
			}

		protected:
			void operator = ( const PickerVisitor & )
			{
			}

		public:
			void visit( const RenderViewportInterfacePtr & _viewport, const RenderCameraInterfacePtr & _camera, const NodePtr & _node )
			{
				RenderViewportInterfacePtr nodeViewport = _node->getRenderViewport();
							
				if( nodeViewport == nullptr )
				{
					nodeViewport = _viewport;
				}

                RenderCameraInterfacePtr nodeCamera = _node->getRenderCamera();

				if( nodeCamera == nullptr )
				{
					nodeCamera = _camera;
				}

				m_currentViewport = nodeViewport;
				m_currentCamera = nodeCamera;
				
				this->accept( _node );

				TListNodeChild & child = _node->getChildren();

				NodePtr single_child = child.single();

				if( single_child != nullptr )
				{
					this->visit( nodeViewport, nodeCamera, single_child );
				}
				else if( child.countSlugs() == 0 )
				{
					for( TListNodeChild::unslug_iterator 
						it = child.ubegin(),
						it_end = child.uend();
					it != it_end;
					++it )
					{
						NodePtr children = (*it);

						this->visit( nodeViewport, nodeCamera, children );
					}
				}
				else
				{
					for( TSlugChild it(child); it.eof() == false; )
					{
						NodePtr children = (*it);

						it.next_shuffle();

						this->visit( nodeViewport, nodeCamera, children );
					}
				}
			}

		protected:
			void accept( const NodePtr & _node )
			{
                MousePickerTrapInterfacePtr trap = _node->getPickerTrap();

				if( trap == nullptr )
				{
					return;
				}

				PickerTrapState * trapState = trap->propagatePickerTrapState();

				if( trapState == nullptr )
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
			TVectorPickerTrapStates & m_traps;

			RenderViewportInterfacePtr m_currentViewport;
			RenderCameraInterfacePtr m_currentCamera;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	MousePickerSystem::MousePickerSystem()
		: m_enumerator(0)
		, m_pickerTrapCount(0)
		, m_block( false )
		, m_handleValue( true )
		, m_invalidateTraps( false )
	{
	}
    //////////////////////////////////////////////////////////////////////////
    MousePickerSystem::~MousePickerSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MousePickerSystem::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MousePickerSystem::_finalizeService()
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
	void MousePickerSystem::setBlock( bool _value )
	{
		m_block = _value;
	}
    //////////////////////////////////////////////////////////////////////////
    void MousePickerSystem::setHandleValue( bool _value )
    {
        m_handleValue = _value;
    }
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::setArrow( const ArrowPtr & _arrow )
	{
		m_arrow = _arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::setScene( const ScenePtr & _scene )
	{
		m_scene = _scene;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::setRenderViewport( const RenderViewportInterfacePtr & _viewport )
	{
		m_viewport = _viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::setRenderCamera( const RenderCameraInterfacePtr & _camera )
	{
		m_camera = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::setRenderScissor( const RenderScissorInterfacePtr & _scissor )
	{
		m_scissor = _scissor;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::pickTrap( const mt::vec2f& _point, TVectorPickerTraps & _traps )
	{
		TVectorPickerTrapStates states;
		states.reserve(m_pickerTrapCount);

		if( this->proccesTraps_( _point.x, _point.y, states ) == false )
		{
			return false;
		}

		for( TVectorPickerTrapStates::reverse_iterator
			it = states.rbegin(),
			it_end = states.rend();
		it != it_end;
		++it)
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
			
			const MousePickerTrapInterfacePtr & trap = state->trap;
			_traps.emplace_back( trap );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::update()
	{
		if( m_invalidateTraps == true )
		{
			this->updateTraps();

			m_invalidateTraps = false;
		}

		this->updateDead_();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::clear()
	{
		this->updateDead_();
	}
	//////////////////////////////////////////////////////////////////////////
	PickerTrapState * MousePickerSystem::regTrap( const MousePickerTrapInterfacePtr & _trap )
	{
		PickerTrapState state;

		state.trap = _trap;

		uint32_t id = ++m_enumerator;
		state.id = id;
		state.picked = false;
		state.pressed = false;
		state.handle = false;
		state.dead = false;

		m_pickerTrapState.emplace_back( state );
		PickerTrapState & refState = m_pickerTrapState.back();

		++m_pickerTrapCount;
		
		return &refState;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::updateTraps()
	{
		const mt::vec2f & pos = INPUT_SERVICE()
			->getCursorPosition( 0 );

		TVectorPickerTrapStates states;
		states.reserve(m_pickerTrapCount);

		this->proccesTraps_( pos.x, pos.y, states );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::invalidateTraps()
	{
		m_invalidateTraps = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::unregTrap( PickerTrapState * _ref )
	{
		for( TPickerTrapState::iterator
			it = m_pickerTrapState.begin(),
			it_end = m_pickerTrapState.end();
		it != it_end;
		++it)
		{
            PickerTrapState & state = *it;

			if( state.id == _ref->id )
			{
				state.dead = true;
				//it->picked = false;

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
	bool MousePickerSystem::handleKeyEvent( const InputKeyEvent & _event )
	{
		m_states.clear();

		if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
		{
			return false;
		}

		for( TVectorPickerTrapStates::reverse_iterator
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
				
			const MousePickerTrapInterfacePtr & trap = state->trap;

			if( trap->handleKeyEvent( _event ) == false )
			{
				continue;
			}
			
			return m_handleValue;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleTextEvent( const InputTextEvent & _event )
	{
		m_states.clear();

		if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
		{
			return false;
		}

		for( TVectorPickerTrapStates::reverse_iterator
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

            const MousePickerTrapInterfacePtr & trap = state->trap;

			if( trap->handleTextEvent( _event ) == false )
			{
				continue;
			}

			return m_handleValue;
		}

		return false;
	}	
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
	{
		m_states.clear();

		if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
		{
			return false;
		}

		for( TVectorPickerTrapStates::reverse_iterator
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
			
            const MousePickerTrapInterfacePtr & trap = state->trap;

			const RenderViewportInterfacePtr & viewport = desc.viewport;
			const RenderCameraInterfacePtr & camera = desc.camera;

			mt::vec2f wp;
			m_arrow->calcPointClick( camera, viewport, mt::vec2f(_event.x, _event.y), wp );

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
	bool MousePickerSystem::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
	{
		m_states.clear();

		if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
		{
			return false;
		}

		for( TVectorPickerTrapStates::reverse_iterator
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
			
            const MousePickerTrapInterfacePtr & trap = state->trap;

			const RenderViewportInterfacePtr & viewport = desc.viewport;
			const RenderCameraInterfacePtr & camera = desc.camera;

			mt::vec2f wp;
			m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), wp );

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
	bool MousePickerSystem::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
	{
		m_states.clear();

		if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
		{
			return false;
		}

		for( TVectorPickerTrapStates::reverse_iterator
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
			
            const MousePickerTrapInterfacePtr & trap = state->trap;

			const RenderViewportInterfacePtr & viewport = desc.viewport;
			const RenderCameraInterfacePtr & camera = desc.camera;

			mt::vec2f wp;
			m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), wp );

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
	bool MousePickerSystem::handleMouseMove( const InputMouseMoveEvent & _event )
	{
		m_states.clear();

		if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
		{
			return false;
		}

		for( TVectorPickerTrapStates::reverse_iterator
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
			
            const MousePickerTrapInterfacePtr & trap = state->trap;

			const RenderViewportInterfacePtr & viewport = desc.viewport;
			const RenderCameraInterfacePtr & camera = desc.camera;

			mt::vec2f wp;
			m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y), wp );

			mt::vec2f dp;
			m_arrow->calcPointDeltha( camera, mt::vec2f( _event.x, _event.y ), mt::vec2f( _event.dx, _event.dy ), dp );

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
	bool MousePickerSystem::handleMouseWheel( const InputMouseWheelEvent & _event )
	{
		m_states.clear();

		if( this->proccesTraps_( _event.x, _event.y, m_states ) == false )
		{
			return false;
		}

		for( TVectorPickerTrapStates::reverse_iterator
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

            const MousePickerTrapInterfacePtr & trap = state->trap;

			const RenderViewportInterfacePtr & viewport = desc.viewport;
			const RenderCameraInterfacePtr & camera = desc.camera;

			mt::vec2f wp;
			m_arrow->calcPointClick( camera, viewport, mt::vec2f( _event.x, _event.y ), wp );

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
	void MousePickerSystem::handleMouseEnter( const InputMousePositionEvent & _event )
	{
		m_states.clear();

		this->proccesTraps_( _event.x, _event.y, m_states );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::handleMouseLeave( const InputMousePositionEvent & _event )
	{
		(void)_event;

		if( m_arrow == nullptr )
		{
			return;
		}

		if( m_scene == nullptr )
		{
			return;
		}

		m_states.clear();

		PickerVisitor pv(m_states);
		pv.visit( m_viewport, m_camera, m_scene );

		for( TVectorPickerTrapStates::reverse_iterator
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

            const MousePickerTrapInterfacePtr & trap = state->trap;

			if( state->picked == true )
			{
				state->picked = false;

				trap->onHandleMouseLeave();
			}
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::proccesTraps_( float _x, float _y, TVectorPickerTrapStates & _states )
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

        if( INPUT_SERVICE()
            ->validCursorPosition( _x, _y ) == false )
        {		
            return false;
        }
				
		PickerVisitor pv(_states);
		pv.visit( m_viewport, m_camera, m_scene );

		const Resolution & contentResolution = APPLICATION_SERVICE()
			->getContentResolution();

		mt::vec2f adapt_screen_position;
		m_arrow->adaptScreenPosition_( mt::vec2f( _x, _y ), adapt_screen_position );
        
		bool handle = false;

		for( TVectorPickerTrapStates::reverse_iterator
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

            const MousePickerTrapInterfacePtr & trap = state->trap;

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
	void MousePickerSystem::updateDead_()
	{
		for( TPickerTrapState::iterator
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
	uint32_t MousePickerSystem::getPickerTrapCount() const
	{
		uint32_t count = 0;

		for( TPickerTrapState::const_iterator
			it = m_pickerTrapState.begin();
			it != m_pickerTrapState.end();
		++it )
		{
			const PickerTrapState & state = *it;

			if( state.dead == true )
			{
				continue;
			}

			++count;
		}
		
		return count;
	}
}
