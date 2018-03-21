#pragma once

#include "Kernel/Node.h"

#include "pybind/object.hpp"

#include "math/vec3.h"

namespace Mengine
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

		void setDead();
		bool isDead() const;

		void setCollide( bool _value );
		bool isCollide() const;

		void setBound( bool _value );
		bool isBound() const;

	public:
		void initialize( Node * _node, const mt::vec3f & _position, const mt::vec3f & _velocity, float _radius, bool _collide, const pybind::object & _cb );

	public:
		void setVelocity( const mt::vec3f & _velocity );
		const mt::vec3f & getVelocity() const;

	public:
		void translate( const mt::vec3f & _translate );

	public:
		bool check_collision( float _timing, const mt::vec3f & _burritoPosition, float _burritoRadius, const mt::vec3f & _burritoVelocity, float & _collisionTiming ) const;

	protected:		
		Node * m_node;
		
		mt::vec3f m_position;
		mt::vec3f m_velocity;
		float m_radius;

		pybind::object m_cb;

		mutable bool m_dead;
		mutable bool m_collide;
		mutable bool m_bound;
	};
}