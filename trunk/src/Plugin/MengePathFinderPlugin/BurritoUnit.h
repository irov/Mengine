#	pragma once

#	include "Kernel/Node.h"

#	include "Math/vec3.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class BurritoUnit
	{
	public:
		BurritoUnit();
		~BurritoUnit();

	public:
		void initialize( Node * _node, float _radius );

	public:
		void setVelocity( const mt::vec3f & _velocity );
		const mt::vec3f & getVelocity() const;

	protected:		
		Node * m_node;
		float m_radius;

		mt::vec3f m_velocity;
	};
}