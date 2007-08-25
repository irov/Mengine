#	include "StackFSM.h"
#	include "State.h"

#	include <algorithm>

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	StackFSM::StackFSM()
		: m_currentAction(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	StackFSM::update( float _timing )
	{
		if(m_stateList.empty())
		{
			return;
		}

		m_currentAction->update( _timing );

		if(m_currentAction->isEnded())
		{
			m_currentAction->end();

			//зовем колбек для скрипта!

			delete m_stateList.front();
			m_stateList.pop_front();

			if(m_stateList.empty())
			{
				m_currentAction = NULL;
				return;
			}

			m_currentAction = m_stateList.front();
			m_currentAction->run();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	StackFSM::addState( State * _action )
	{
		if(m_stateList.empty())
		{
			m_currentAction = _action;
			_action->run();
		}

		m_stateList.push_back(_action);
	}
	//////////////////////////////////////////////////////////////////////////
	void	StackFSM::runState( State * _action )
	{
		if(m_stateList.empty())
		{
			addState(_action);
			return;
		}


		StateList::iterator it = m_stateList.begin();
	/*	
		m_currentAction->end();

		if(m_currentAction->isEnded() == false)
		{
			it++;
		}
	*/
		std::for_each(it, m_stateList.end(), std::mem_fun(&State::end));
		m_stateList.erase(it, m_stateList.end());
	
		addState(_action);
	}
	//////////////////////////////////////////////////////////////////////////
	void	StackFSM::terminateStates()
	{
		StateList::iterator it = m_stateList.begin();
		std::for_each(it, m_stateList.end(), std::mem_fun(&State::end));
		m_stateList.erase(it, m_stateList.end());
	}
	//////////////////////////////////////////////////////////////////////////
}