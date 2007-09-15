#	pragma once

#	include "State.h"

#	include "math\vec3.h"

namespace Menge
{
	class	Avatar3D;

	class	StateMove3D : public State
	{
	public:
		StateMove3D( Avatar3D * _avatar, const mt::vec3f& _destPoint );
	public:
		void	update( float _timing );
		void	run();
		void	end();
		bool	isEnded() const;
	private:
		Avatar3D *	m_avatar;
		mt::vec3f	m_destPoint;
		bool		m_isEnded;
	};
}