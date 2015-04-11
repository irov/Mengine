#	pragma once

#	include "Kernel/Node.h"

#	include "Core/ConstString.h"

#	include "Math/vec3.h"

#	include "stdex/stl_vector.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct BurritoBisonForce
	{
		ConstString name;
		mt::vec3f direction;
		float value;
	};
	//////////////////////////////////////////////////////////////////////////
	struct BurritoBisonImpulse
	{
		mt::vec3f direction;
		float value;
		float time;

		mutable float timing;
	};
	//////////////////////////////////////////////////////////////////////////
	class BurritoBison
	{
	public:
		BurritoBison();
		~BurritoBison();

	public:
		void initialize( Node * _node, float _radius );

	public:
		void addForce( const ConstString & _name, const mt::vec3f & _direction, float _value );
		void removeForce( const ConstString & _name );

		void addImpulse( const mt::vec3f & _direction, float _value, float _time );

	public:
		float getRadius() const;
		const mt::vec3f & getVelocity() const;

	public:
		void update_velocity( float _time, float _timing, mt::vec3f & _velocity );
		void reflect_velocity( float _factor );

	protected:		
		Node * m_node;
		float m_radius;

		mt::vec3f m_velocity;

		typedef stdex::vector<BurritoBisonForce> TVectorBurritoBisonForce;
		TVectorBurritoBisonForce m_forces;

		typedef stdex::vector<BurritoBisonImpulse> TVectorBurritoBisonImpulse;
		TVectorBurritoBisonImpulse m_impulses;
	};
}