#	include "ActionMove.h"	

#	include "Avatar3D.h"	

namespace Menge
{ 
	//////////////////////////////////////////////////////////////////////////
	ActionMove::ActionMove(Avatar3D * _avatar, const mt::vec3f& _destPoint)
		: Action()
		, m_avatar(_avatar)
		, m_destPoint(_destPoint)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	ActionMove::update( float _timing )
	{
		if(m_avatar->isMoving() == false)
		{
			m_isEnded = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	ActionMove::run()
	{
		printf("ActionMove is running! \n");
		m_avatar->moveTo(m_destPoint);
	}
	//////////////////////////////////////////////////////////////////////////
	void	ActionMove::end()
	{
		printf("ActionMove is ended! \n");
		m_avatar->stop();
	}
	//////////////////////////////////////////////////////////////////////////
}