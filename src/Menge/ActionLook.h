#	pragma once

#	include "Action.h"

#	include "math\vec3.h"

namespace Menge
{
	class	Avatar3D;

	class	ActionLook : public Action
	{
	public:
		ActionLook(Avatar3D * _avatar, const mt::vec3f& _destPoint);
	public:
		void	update( float _timing );
		void	run();
		void	end();
	private:
		Avatar3D *	m_avatar;
		mt::vec3f	m_destPoint;
	};
}