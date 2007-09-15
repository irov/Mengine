#	include "StateLook2D.h"	

#	include "Avatar2D.h"	

namespace Menge
{ 
	//////////////////////////////////////////////////////////////////////////
	StateLook2D::StateLook2D( Avatar2D * _avatar, const mt::vec2f& _destPoint )
		: m_avatar( _avatar )
		, m_destPoint( _destPoint )
		, m_isEnded( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	StateLook2D::update( float _timing )
	{
		if( m_avatar->isMoving() == false )
		{
			m_isEnded = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	StateLook2D::run()
	{
		printf("StateLook2D is running! \n");
		m_avatar->lookTo(m_destPoint);
	}
	//////////////////////////////////////////////////////////////////////////
	void	StateLook2D::end()
	{
		printf("StateLook2D is ended! \n");
	}
	//////////////////////////////////////////////////////////////////////////
	bool	StateLook2D::isEnded() const
	{
		return m_isEnded;
	}
	//////////////////////////////////////////////////////////////////////////
}