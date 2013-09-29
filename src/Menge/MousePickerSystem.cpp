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
		typedef std::vector<PickerTrapState *> TVectorPickerTrapStates;
		//////////////////////////////////////////////////////////////////////////
		class PickerVisitor
			: public Visitor
			, public ConcreteVisitor<HotSpot>
		{
		public:
			PickerVisitor( TVectorPickerTrapStates & _traps )
				: m_traps(_traps)
			{
			}

		public:
			void visit( Node * _node )
			{
				_node->visit( this );

				TListNodeChild & child = _node->getChild();

				for( TSlugChild it(child); it.eof() == false; it.next_shuffle() )
				{
					Node * children = (*it);

					this->visit( children );
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

				m_traps.push_back( trapState );
			}

		protected:
			TVectorPickerTrapStates & m_traps;
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
	void MousePickerSystem::pickTrap( const mt::vec2f& _point, TVectorPickerTraps & _traps )
	{
		this->proccesTraps_( _point );

		for( TPickerTrapState::reverse_iterator
			it = m_pickerTrapState.rbegin(),
			it_end = m_pickerTrapState.rend();
		it != it_end;
		++it)
		{
			PickerTrapState * state = &*it;

			if( state->dead == true )
			{
				continue;
			}

			if( state->picked == true )
			{
				MousePickerTrapInterface * trap = state->trap;

				_traps.push_back( trap );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::update()
	{
		const mt::vec2f & pos = INPUT_SERVICE(m_serviceProvider)
			->getCursorPosition();

		this->proccesTraps_( pos );

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
		
		const mt::vec2f & pos = INPUT_SERVICE(m_serviceProvider)
			->getCursorPosition();

		this->proccesTraps_( pos );

		return &refState;
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
	bool MousePickerSystem::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		this->proccesTraps_( _point );

		if( m_scene == nullptr )
		{
			return false;
		}

		TVectorPickerTrapStates states;
		PickerVisitor pv(states);
		pv.visit( m_scene );

		for( TVectorPickerTrapStates::reverse_iterator
			it = states.rbegin(),
			it_end = states.rend();
		it != it_end;
		++it )
		{
			PickerTrapState * state = (*it);

			if( state->dead == true )
			{
				continue;
			}
			
			if( MousePickerSystem::isPicked( state ) == true )
			{
				MousePickerTrapInterface * trap = state->trap;

				if( trap->handleKeyEvent( _point, _key, _char, _isDown ) == true )
				{
					return m_handleValue;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		this->proccesTraps_( _point );

		if( m_scene == nullptr )
		{
			return false;
		}

		TVectorPickerTrapStates states;
		PickerVisitor pv(states);
		pv.visit( m_scene );

		for( TVectorPickerTrapStates::reverse_iterator
			it = states.rbegin(),
			it_end = states.rend();
		it != it_end;
		++it )
		{
			PickerTrapState * state = (*it);

			if( state->dead == true )
			{
				continue;
			}
			
			if( MousePickerSystem::isPicked( state ) == true )
			{
				MousePickerTrapInterface * trap = state->trap;

				if( trap->handleMouseButtonEvent( _touchId, _point, _button, _isDown ) == true )
				{
					return m_handleValue;
				}
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		this->proccesTraps_( _point );

		if( m_scene == nullptr )
		{
			return false;
		}

		TVectorPickerTrapStates states;
		PickerVisitor pv(states);
		pv.visit( m_scene );

		for( TVectorPickerTrapStates::reverse_iterator
			it = states.rbegin(),
			it_end = states.rend();
		it != it_end;
		++it )
		{
			PickerTrapState * state = (*it);

			if( state->dead == true )
			{
				continue;
			}

			if( MousePickerSystem::isPicked( state ) == true )
			{
				MousePickerTrapInterface * trap = state->trap;

				if( trap->handleMouseButtonEventBegin( _touchId, _point, _button, _isDown ) == true )
                {
                    return m_handleValue;
                }
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		this->proccesTraps_( _point );

		if( m_scene == nullptr )
		{
			return false;
		}

		TVectorPickerTrapStates states;
		PickerVisitor pv(states);
		pv.visit( m_scene );

		for( TVectorPickerTrapStates::reverse_iterator
			it = states.rbegin(),
			it_end = states.rend();
		it != it_end;
		++it )
		{
			PickerTrapState * state = (*it);

			if( state->dead == true )
			{
				continue;
			}

			if( MousePickerSystem::isPicked( state ) == true )
			{
				MousePickerTrapInterface * trap = state->trap;

				if( trap->handleMouseButtonEventEnd( _touchId, _point, _button, _isDown ) == true )
                {
                    return m_handleValue;
                }
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerSystem::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		this->proccesTraps_( _point );

		if( m_scene == nullptr )
		{
			return false;
		}

		TVectorPickerTrapStates states;
		PickerVisitor pv(states);
		pv.visit( m_scene );

		for( TVectorPickerTrapStates::reverse_iterator
			it = states.rbegin(),
			it_end = states.rend();
		it != it_end;
		++it )
		{
			PickerTrapState * state = (*it);

			if( state->dead == true )
			{
				continue;
			}
						
			if( MousePickerSystem::isPicked( state ) == true )
			{
				MousePickerTrapInterface * trap = state->trap;

				if( trap->handleMouseMove( _touchId, _point, _x, _y, _whell ) == true )
				{
					return m_handleValue;
				}
			}
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerSystem::handleMouseEnter( const mt::vec2f & _point )
	{
		this->proccesTraps_( _point );
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

		TVectorPickerTrapStates states;
		PickerVisitor pv(states);
		pv.visit( m_scene );

		for( TVectorPickerTrapStates::reverse_iterator
			it = states.rbegin(),
			it_end = states.rend();
		it != it_end;
		++it )
		{
			PickerTrapState * state = (*it);

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
	void MousePickerSystem::proccesTraps_( const mt::vec2f& _point )
	{
		if( m_arrow == nullptr )
		{
			return;
		}

		if( m_scene == nullptr )
		{
			return;
		}

        if( INPUT_SERVICE(m_serviceProvider)
            ->validCursorPosition( _point ) == false )
        {		
            return;
        }

		TVectorPickerTrapStates states;
		PickerVisitor pv(states);
		pv.visit( m_scene );
        
		bool handle = false;

		for( TVectorPickerTrapStates::reverse_iterator
			it = states.rbegin(),
			it_end = states.rend();
		it != it_end;
		++it )
		{
			PickerTrapState * state = (*it);

			if( state->dead == true )
			{
				continue;
			}

			MousePickerTrapInterface * trap = state->trap;

			bool pickerActive = trap->isPickerActive();
			bool picked = trap->pick( _point, m_arrow );

			if( ( handle == false || m_handleValue == false ) && m_block == false && pickerActive == true && picked == true )
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
	bool MousePickerSystem::isPicked( const PickerTrapState * _state )
	{
		if( _state->picked == false )
		{
			return false;
		}

		if( _state->trap->isPickerActive() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MousePickerSystem::getPickerTrapCount() const
	{
        size_t count = m_pickerTrapState.size();

		return count;
	}
}
