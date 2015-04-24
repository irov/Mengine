#	pragma once

#	include "Kernel/Node.h"

#	include "Core/ConstString.h"

#	include "Math/vec3.h"

#	include "pybind/object.hpp"

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
	struct VelocityEventDesc
	{
		bool less;
		mt::vec3f velocity;
		float velocity_sqrlength;
		pybind::object cb;

		bool test;
		bool dead;
	};
	//////////////////////////////////////////////////////////////////////////
	class BurritoBison
	{
	public:
		BurritoBison();
		~BurritoBison();

	public:
		void initialize( Node * _node, const mt::vec3f & _offset, float _bisonY, float _radius );

	public:
		void addForce( const ConstString & _name, const mt::vec3f & _direction, float _value );
		void removeForce( const ConstString & _name );

		void addImpulse( const mt::vec3f & _direction, float _value, float _time );

	public:
		void addVelocityEvent( bool _less, const mt::vec3f & _velocity, const pybind::object & _cb );
		void removeAllVelocityEvents();

	public:
		const mt::vec3f & getPosition() const;
		const mt::vec3f & getOffset() const;
		float getRadius() const;
		
		void setVelocity( const mt::vec3f & _velocity );
		const mt::vec3f & getVelocity() const;

	public:
		void update( float _time, float _timing, mt::vec3f & _velocity, mt::vec3f & _offset, mt::vec3f & _offsetH );

	public:
		void translate( const mt::vec3f & _translate, mt::vec3f & _position );

	protected:
		bool testVelocityEvent_( const VelocityEventDesc & _desc ) const;

	protected:		
		Node * m_node;

		mt::vec3f m_offset;
		mt::vec3f m_position;

		float m_bisonY;

		float m_radius;

		mt::vec3f m_velocity;

		typedef stdex::vector<BurritoBisonForce> TVectorBurritoBisonForce;
		TVectorBurritoBisonForce m_forces;

		typedef stdex::vector<BurritoBisonImpulse> TVectorBurritoBisonImpulse;
		TVectorBurritoBisonImpulse m_impulses;

		typedef stdex::vector<VelocityEventDesc> TVectorVelocityEventDesc;
		TVectorVelocityEventDesc m_velocityEvents;
		TVectorVelocityEventDesc m_velocityEventsAdd;
	};
}