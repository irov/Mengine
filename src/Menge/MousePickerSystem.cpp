#	include "MousePickerSystem.h"

#   include "Interface/InputSystemInterface.h"

#	include "Arrow.h"
#	include "Kernel/Scene.h"
#	include "Player.h"

#	include "HotSpot.h"

#	include "Logger/Logger.h"

#	include <algorithm>

namespace Menge
{
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		class PickerVisitor
			: public Visitor
			, public ConcreteVisitor<HotSpot>
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
			void visit( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, Node * _node )
			{
				const RenderViewportInterface * nodeViewport = _node->getRenderViewport();
				const RenderCameraInterface * nodeCamera = _node->getRenderCamera();
							
				if( nodeViewport == nullptr )
				{
					nodeViewport = _viewport;
				}

				if( nodeCamera == nullptr )
				{
					nodeCamera = _camera;
				}

				m_currentViewport = nodeViewport;
				m_currentCamera = nodeCamera;
				
				_node->visit( this );

				TListNodeChild & child = _node->getChild();

				for( TSlugChild it(child); it.eof() == false; it.next_shuffle() )
				{
					Node * children = (*it);

					this->visit( nodeViewport, nodeCamera, children );
				}
			}

		protected:
			void accept( HotSpot * _visited ) override
			{
				MousePickerTrapInterface * trap = _visited->getPickerTrap();

				PickerTrapState * trapState = trap->getPicker();

				if( trapState == nullptr )
				{
					return;
				}

				PickerTrapStateDesc desc;
				desc.state = trapState;
				desc.viewport = m_currentViewport;
				desc.camera = m_currentCamera;

				m_traps.push_back( desc );
			}

		protected:
			TVectorPickerTrapStates & m_traps;

			const RenderViewportInterface * m_currentViewport;
			const RenderCameraInterface * m_currentCamera;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	MousePickerSystem::MousePickerSystem( ServiceProviderInterface * _serviceProvider )
		: m_serviceProvider(_serviceProvider)
        , m_enumerator(0)
		, m_block(false)
        , m_handleValue(true)
		, m_arrow(nullptr)
		, m_scene(nullptr)
		, m_viewport(nullptr)
		, m_camera(nullptr)
		, m_pickerTrapCount(0)
	{
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
	void MousePickerSystem::setArrow( Arrow * _arrow )
	{
		m_arrow = _arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::setScene( Scene * _scene )
	{
		m_scene = _scene;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::setRenderViewport( const RenderViewportInterface * _viewport )
	{
		m_viewport = _viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::setRenderCamera( const RenderCameraInterface * _camera )
	{
		m_camera = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::pickTrap( const mt::vec2f& _point, TVectorPickerTraps & _traps )
	{
		TVectorPickerTrapStates states;
		states.reserve(m_pickerTrapCount);

		if( this->proccesTraps_( _point, states ) == false )
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
			
			MousePickerTrapInterface * trap = state->trap;					
			_traps.push_back( trap );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::update()
	{
		//const mt::vec2f & pos = INPUT_SERVICE(m_serviceProvider)
		//	->getCursorPosition();

		//this->proccesTraps_( pos );

		this->updateDead_();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::clear()
	{
		this->updateDead_();
	}
	//////////////////////////////////////////////////////////////////////////
	PickerTrapState * MousePickerSystem::regTrap( MousePickerTrapInterface * _trap )
	{
		size_t id = ++m_enumerator;

		PickerTrapState state;

		state.trap = _trap;
		state.id = id;
		state.picked = false;
		state.handle = false;
		state.dead = false;

		m_pickerTrapState.push_back( state );
		PickerTrapState & refState = m_pickerTrapState.back();

		++m_pickerTrapCount;
		
		return &refState;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::updateTrap()
	{
		const mt::vec2f & pos = INPUT_SERVICE(m_serviceProvider)
			->getCursorPosition();

		TVectorPickerTrapStates states;
		states.reserve(m_pickerTrapCount);

		this->proccesTraps_( pos, states );
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

				if( state.picked == true )
				{
					state.picked = false;
					state.trap->onMouseLeave();
				}

				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating )
	{
		m_states.clear();
		if( this->proccesTraps_( _point, m_states ) == false )
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
				
			MousePickerTrapInterface * trap = state->trap;

			if( trap->handleKeyEvent( _point, _key, _char, _isDown, _repeating ) == false )
			{
				continue;
			}
			
			return m_handleValue;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		m_states.clear();
		if( this->proccesTraps_( _point, m_states ) == false )
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
			
			MousePickerTrapInterface * trap = state->trap;

			const RenderViewportInterface * viewport = desc.viewport;
			const RenderCameraInterface * camera = desc.camera;

			mt::vec2f wp;
			m_arrow->calcPointClick( camera, viewport, _point, wp );

			if( trap->handleMouseButtonEvent( _touchId, wp, _button, _isDown ) == false )
			{
				continue;
			}

			return m_handleValue;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		m_states.clear();
		if( this->proccesTraps_( _point, m_states ) == false )
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
			
			MousePickerTrapInterface * trap = state->trap;

			const RenderViewportInterface * viewport = desc.viewport;
			const RenderCameraInterface * camera = desc.camera;

			mt::vec2f wp;
			m_arrow->calcPointClick( camera, viewport, _point, wp );
			
			if( trap->handleMouseButtonEventBegin( _touchId, wp, _button, _isDown ) == false )			
			{
				continue;
			}
             
			return m_handleValue;            
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		m_states.clear();
		if( this->proccesTraps_( _point, m_states ) == false )
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
			
			MousePickerTrapInterface * trap = state->trap;

			const RenderViewportInterface * viewport = desc.viewport;
			const RenderCameraInterface * camera = desc.camera;

			mt::vec2f wp;
			m_arrow->calcPointClick( camera, viewport, _point, wp );

			if( trap->handleMouseButtonEventEnd( _touchId, wp, _button, _isDown ) == false )
			{
				continue;
			}
             
			return m_handleValue;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y )
	{
		m_states.clear();
		if( this->proccesTraps_( _point, m_states ) == false )
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
			
			MousePickerTrapInterface * trap = state->trap;

			const RenderViewportInterface * viewport = desc.viewport;
			const RenderCameraInterface * camera = desc.camera;

			mt::vec2f wp;
			m_arrow->calcPointClick( camera, viewport, _point, wp );

			mt::vec2f deltha;
			m_arrow->calcPointDeltha( camera, _point, mt::vec2f(_x, _y), deltha );


			if( trap->handleMouseMove( _touchId, wp, deltha.x, deltha.y ) == false )
			{
				continue;
			}

			return m_handleValue;		
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel )
	{
		m_states.clear();
		if( this->proccesTraps_( _point, m_states ) == false )
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

			MousePickerTrapInterface * trap = state->trap;

			const RenderViewportInterface * viewport = desc.viewport;
			const RenderCameraInterface * camera = desc.camera;

			mt::vec2f wp;
			m_arrow->calcPointClick( camera, viewport, _point, wp );

			if( trap->handleMouseWheel( _touchId, wp, _wheel ) == false )
			{
				continue;
			}

			return m_handleValue;		
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::handleMouseEnter( const mt::vec2f & _point )
	{
		m_states.clear();
		this->proccesTraps_( _point, m_states );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::handleMouseLeave()
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

			MousePickerTrapInterface * trap = state->trap;

			if( state->picked == true )
			{
				state->picked = false;

				trap->onMouseLeave();
			}
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::proccesTraps_( const mt::vec2f& _point, TVectorPickerTrapStates & _states )
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

        if( INPUT_SERVICE(m_serviceProvider)
            ->validCursorPosition( _point ) == false )
        {		
            return false;
        }
				
		PickerVisitor pv(_states);
		pv.visit( m_viewport, m_camera, m_scene );
        
		bool handle = false;

		for( TVectorPickerTrapStates::reverse_iterator
			it = _states.rbegin(),
			it_end = _states.rend();
		it != it_end;
		++it )
		{
			PickerTrapState * state = it->state;

			if( state->dead == true )
			{
				continue;
			}

			MousePickerTrapInterface * trap = state->trap;

			const RenderViewportInterface * viewport = it->viewport;
			const RenderCameraInterface * camera = it->camera;
			bool picked = trap->pick( _point, viewport, camera, m_arrow );

			if( ( handle == false || m_handleValue == false ) && m_block == false && picked == true )
			{
				if( state->picked == false )
				{
					state->picked = true;

					handle = trap->onMouseEnter( _point );
									
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
					trap->onMouseLeave();
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

				--m_pickerTrapCount;
			}
			else
			{
				++it;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MousePickerSystem::getPickerTrapCount() const
	{
		size_t count = 0;

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
