#	pragma once

#	include "Math/utils.h"

namespace Menge
{
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

	template <typename T, T (*L)(T, T)>
	class ValueFollowerLinear
		: public ValueFollower<T>
	{
	public:
		ValueFollowerLinear()
			: m_speed(0.f)
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

	protected:
		bool _update( float _timing ) override
		{
			float l = L( m_follow, m_value );

			if( mt::equal_f_z( l ) == true )
			{
				m_value = m_follow;

				return true;
			}

			float step = m_speed * _timing;

			if( step >= l )
			{
				m_value = m_follow;

				return true;
			}

			T offset = m_follow - m_value;

			T add = offset * (step / l);

			m_value += add;

			return false;
		}

	protected:
		float m_speed;
	};
}
