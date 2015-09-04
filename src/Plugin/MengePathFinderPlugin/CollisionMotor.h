#	pragma once

#	include "pybind/pybind.hpp"

namespace Menge
{
	class CollisionObject;

	class CollisionMotor
	{
	public:
		CollisionMotor();
		~CollisionMotor();
		
	public:
		void setCallback( const pybind::object & _cb );

	public:
		void setLinearSpeed( float _speed );
		float getLinearSpeed() const;

		void setAngularSpeed( float _speed );
		float getAngularSpeed() const;

		void setMoveStop( bool _value );
		bool getMoveStop() const;

	public:
		virtual void update( float _timing, CollisionObject * _object ) = 0;

	protected:
		void updateState_( CollisionObject * _object, bool _rotating, bool _transiting, bool _finish );

	protected:
		float m_linearSpeed;
		float m_angularSpeed;

		bool m_moveStop;

		bool m_rotating;
		bool m_transiting;
		bool m_finish;

		pybind::object m_cb;
	};	
}
