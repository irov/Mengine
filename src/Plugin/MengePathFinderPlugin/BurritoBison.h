#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Camera2D.h"

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
	struct DistanceEventDesc
	{
		float init_distance;
		float distance;
		pybind::object cb;

		bool dead;
	};
	//////////////////////////////////////////////////////////////////////////
	class BurritoBison
	{
	public:
		BurritoBison();
		~BurritoBison();

	public:
		void initialize( Node * _node, Camera2D * _camera, const Viewport & _renderport, const mt::vec3f & _position, float _radius );
		void finalize();

	public:
		bool addForce( const ConstString & _name, const mt::vec3f & _direction, float _value );
		bool removeForce( const ConstString & _name );

		void addImpulse( const mt::vec3f & _direction, float _value, float _time );

	public:
		void addVelocityEvent( bool _less, const mt::vec3f & _velocity, const pybind::object & _cb );
		void removeAllVelocityEvents();

		void addDistanceEvent( float _distance, const pybind::object & _cb );
		void removeAllDistanceEvents();

	public:
		Node * getNode() const;
		const mt::vec3f & getPosition() const;
		float getRadius() const;

		void setPositionY( float _position );
		
		void setVelocity( const mt::vec3f & _velocity );
		const mt::vec3f & getVelocity() const;

		void setNeutron( bool _value );
		bool getNeutron() const;

		void setCollide( bool _value );
		bool getCollide() const;

	public:
		void update( float _time, float _timing, mt::vec3f & _velocity, mt::vec3f & _position, uint32_t _iterate );

	public:
		void translate( const mt::vec3f & _translate );

	protected:
		bool testVelocityEvent_( const VelocityEventDesc & _desc ) const;

	protected:		
		Node * m_node;
		Camera2D * m_camera;

		Viewport m_renderport;

		mt::vec3f m_position;

		float m_radius;

		mt::vec3f m_velocity;

		typedef stdex::vector<BurritoBisonForce> TVectorBurritoBisonForce;
		TVectorBurritoBisonForce m_forces;

		typedef stdex::vector<BurritoBisonImpulse> TVectorBurritoBisonImpulse;
		TVectorBurritoBisonImpulse m_impulses;

		typedef stdex::vector<VelocityEventDesc> TVectorVelocityEventDesc;
		TVectorVelocityEventDesc m_velocityEvents;
		TVectorVelocityEventDesc m_velocityEventsAdd;

		typedef stdex::vector<DistanceEventDesc> TVectorDistanceEventDesc;
		TVectorDistanceEventDesc m_distanceEvents;
		TVectorDistanceEventDesc m_distanceEventsAdd;

		bool m_neutron;
		bool m_collide;
	};
}