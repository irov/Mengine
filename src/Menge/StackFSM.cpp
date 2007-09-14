#	include "StackFSM.h"
#	include "State.h"

#	include <algorithm>

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	StackFSM::StackFSM()
		: m_currentState( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	StackFSM::~StackFSM()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	StackFSM::update( float _timing )
	{
		if( m_stateList.empty() )
		{
			return;
		}

		m_currentState->update( _timing );

		if( m_currentState->isEnded() )
		{
			m_currentState->end();

			//FIX ME:
			//SCRIPT CALLBACK!!!!!!!!!!!!

			pop();

			if( m_stateList.empty() )
			{
				m_currentState = NULL;
				return;
			}

			next();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	StackFSM::pop()
	{
		delete m_stateList.front();
		m_stateList.pop_front();
	}
	//////////////////////////////////////////////////////////////////////////
	void	StackFSM::next()
	{
		m_currentState = m_stateList.front();
		m_currentState->run();
	}
	//////////////////////////////////////////////////////////////////////////
	void	StackFSM::push( State * _state )
	{
		if( m_stateList.empty() )
		{
			m_currentState = _state;
			_state->run();
		}

		m_stateList.push_back(_state);
	}
	//////////////////////////////////////////////////////////////////////////
	void	StackFSM::execute( State * _state )
	{
		terminate();
		push( _state );
	}
	//////////////////////////////////////////////////////////////////////////
	void	StackFSM::terminate()
	{
		struct	TerminateState
		{
			void operator()( State * _state )
			{
				_state->end();
				delete _state;
			}
		};

		std::for_each( m_stateList.begin(), m_stateList.end(), TerminateState() );
		m_stateList.clear();
	}
	//////////////////////////////////////////////////////////////////////////
}