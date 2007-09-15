#	pragma once

#	include "State.h"

#	include "math\vec2.h"

namespace Menge
{
	class	Avatar2D;

	class	StateLook2D : public State
	{
	public:
		StateLook2D( Avatar2D * _avatar, const mt::vec2f& _destPoint );
	public:
		void	update( float _timing );
		void	run();
		void	end();
		bool	isEnded() const;
	private:
		Avatar2D *	m_avatar;
		mt::vec2f	m_destPoint;
		bool		m_isEnded;
	};
}