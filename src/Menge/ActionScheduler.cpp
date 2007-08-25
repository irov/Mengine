#	include "ActionScheduler.h"
#	include "Action.h"

#	include <algorithm>

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	ActionScheduler::ActionScheduler()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	ActionScheduler::update( float _timing )
	{
		if(m_actionList.empty())
		{
			return;
		}

		m_currentAction->update(_timing);

		if(m_currentAction->isEnded())
		{
			m_currentAction->end();

			//зовем колбек для скрипта!

			delete m_actionList.front();
			m_actionList.pop_front();

			if(m_actionList.empty())
			{
				m_currentAction = NULL;
				return;
			}

			m_currentAction = m_actionList.front();
			m_currentAction->run();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	ActionScheduler::addAction( Action * _action )
	{
		if(m_actionList.empty())
		{
			m_currentAction = _action;
			_action->run();
		}

		m_actionList.push_back(_action);
	}
	//////////////////////////////////////////////////////////////////////////
	void	ActionScheduler::runAction( Action * _action )
	{
		if(m_actionList.empty())
		{
			addAction(_action);
			return;
		}


		ActionList::iterator it = m_actionList.begin();


	/*	
		m_currentAction->end();

		if(m_currentAction->isEnded() == false)
		{
			it++;
		}
	*/
		std::for_each(it, m_actionList.end(), std::mem_fun(&Action::end));
		m_actionList.erase(it, m_actionList.end());
	
		addAction(_action);
	}
	//////////////////////////////////////////////////////////////////////////
}