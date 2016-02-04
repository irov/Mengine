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

		const T & getValue() const
		{
			return m_value;
		}

		const T & getFollow() const
		{
			return m_follow;
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
			: m_speed(0.f)
			, m_distance(0.f)
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
			float l = mt::length( ValueFollower<T>::m_follow, ValueFollower<T>::m_value );

			if( mt::equal_f_z( l ) == true )
			{
				ValueFollower<T>::m_value = ValueFollower<T>::m_follow;

				return true;
			}

			float step = m_speed * _timing;

			if( step >= l )
			{
				ValueFollower<T>::m_value = ValueFollower<T>::m_follow;

				return true;
			}
			else if( step + m_distance >= l )
			{
				T offset = ValueFollower<T>::m_follow - ValueFollower<T>::m_value;

				T add = offset * ((l - m_distance) / l);

				ValueFollower<T>::m_value += add;

				return true;
			}

			T offset = ValueFollower<T>::m_follow - ValueFollower<T>::m_value;

			T add = offset * (step / l);

			ValueFollower<T>::m_value += add;

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
			float l = mt::length( ValueFollower<T>::m_follow, ValueFollower<T>::m_value );

			if( mt::equal_f_z( l ) == true )
			{
				ValueFollower<T>::m_value = ValueFollower<T>::m_follow;

				return true;
			}

			m_speed += m_acceleration * _timing;

			float step = m_speed * _timing;

			if( step >= l )
			{
				ValueFollower<T>::m_value = ValueFollower<T>::m_follow;

				return true;
			}
			else if( step + m_distance >= l )
			{
				T offset = ValueFollower<T>::m_follow - ValueFollower<T>::m_value;

				T add = offset * ((l - m_distance) / l);

				ValueFollower<T>::m_value += add;

				return true;
			}

			T offset = ValueFollower<T>::m_follow - ValueFollower<T>::m_value;

			T add = offset * (step / l);

			ValueFollower<T>::m_value += add;

			return false;
		}

	protected:
		float m_speed;
		float m_acceleration;
		float m_distance;
	};
}
