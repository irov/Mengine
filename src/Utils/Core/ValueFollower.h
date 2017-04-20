#	pragma once

#	include "Math/utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	template <typename T>
	class ValueFollower
	{
	public:
		void initialize( const T & _value )
		{
			m_value = _value;
			m_follow = _value;
		}

		void setValue( const T & _value )
		{
			m_value = _value;
		}

		const T & getValue() const
		{
			return m_value;
		}

		const T & getFollow() const
		{
			return m_follow;
		}

		T getDistance() const
		{
			T distance = m_follow - m_value;

			return distance;
		}

		void overtake()
		{
			m_value = m_follow;
		}

		void step( const T & _value )
		{
			m_value += _value;
		}

		float getLength() const
		{
			float length = mt::length( m_follow, m_value );

			return length;
		}

		void follow( const T & _follow )
		{
			m_follow = _follow;
		}

		bool update( float _timing )
		{
			bool successful = this->_update( _timing );

			return successful;
		}

	protected:
		virtual bool _update( float _timing ) = 0;

	protected:
		T m_value;
		T m_follow;
	};
	//////////////////////////////////////////////////////////////////////////
	template <typename T>
	class ValueFollowerLinear
		: public ValueFollower<T>
	{
	public:
		ValueFollowerLinear()
			: m_speed( 0.f )
			, m_distance( 0.f )
		{
		}

	public:
		void setSpeed( float _speed )
		{
			m_speed = _speed;
		}

		float getSpeed() const
		{
			return m_speed;
		}

		void setDistance( float _distance )
		{
			m_distance = _distance;
		}

		float getDistance() const
		{
			return m_distance;
		}

	protected:
		bool _update( float _timing ) override
		{
			float l = this->getLength();

			if( mt::equal_f_z( l ) == true )
			{
				this->overtake();

				return true;
			}

			float step = m_speed * _timing;

			if( step >= l )
			{
				this->overtake();

				return true;
			}
			else if( step + m_distance >= l )
			{
				T offset = this->getDistance();

				T add = offset * ((l - m_distance) / l);

				this->step( add );

				return true;
			}

			T offset = this->getDistance();

			T add = offset * (step / l);

			this->step( add );

			return false;
		}

	protected:
		float m_speed;
		float m_distance;
	};
	//////////////////////////////////////////////////////////////////////////
	template <typename T>
	class ValueFollowerAcceleration
		: public ValueFollower<T>
	{
	public:
		ValueFollowerAcceleration()
			: m_speed( 0.f )
			, m_acceleration( 0.f )
			, m_distance( 0.f )
		{
		}

	public:
		void setSpeed( float _speed )
		{
			m_speed = _speed;
		}

		float getSpeed() const
		{
			return m_speed;
		}

		void setAcceleration( float _acceleration )
		{
			m_acceleration = _acceleration;
		}

		float getAcceleration() const
		{
			return m_acceleration;
		}

		void setDistance( float _distance )
		{
			m_distance = _distance;
		}

		float getDistance() const
		{
			return m_distance;
		}

	protected:
		bool _update( float _timing ) override
		{
			float l = this->getLength();

			if( mt::equal_f_z( l ) == true )
			{
				this->overtake();

				return true;
			}

			m_speed += m_acceleration * _timing;

			float step = m_speed * _timing;

			if( step >= l )
			{
				this->overtake();

				return true;
			}
			else if( step + m_distance >= l )
			{
				T offset = this->getDistance();

				T add = offset * ((l - m_distance) / l);

				this->step( add );

				return true;
			}

			T offset = this->getDistance();

			T add = offset * (step / l);

			this->step( add );

			return false;
		}

	protected:
		float m_speed;
		float m_acceleration;
		float m_distance;
	};
}
