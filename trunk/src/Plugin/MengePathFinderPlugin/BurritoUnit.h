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
		Node * getNode() const;
		const mt::vec3f & getPosition() const;

	public:
		void initialize( Node * _node, const mt::vec3f & _position, const mt::vec3f & _velocity, float _radius, PyObject * _cb );

	public:
		void setVelocity( const mt::vec3f & _velocity );
		const mt::vec3f & getVelocity() const;

	public:
		void update( float _timing, const mt::vec3f & _translate );

	public:
		bool check_collision( float _timing, const mt::vec3f & _burritoPosition, float _burritoRadius, const mt::vec3f & _burritoVelocity, float & _collisionTiming, mt::vec2f & _factor ) const;

	protected:		
		Node * m_node;
		
		mt::vec3f m_position;
		mt::vec3f m_velocity;
		float m_radius;

		PyObject * m_cb;
	};
}