#	pragma once

namespace Menge
{
	template <class T>
	class ValueInterpolator
	{
	public:
		ValueInterpolator()
			: m_time( 0.0f )
			, m_timing( 0.0f )
			, m_started( false )
		{
		}

		template<class ABS>
		bool start( const T& _value1, const T& _value2, float _time, ABS _abs )
		{
			m_started = false;
			if( _time < 0.00001f || _abs( _value2 - _value1 ) < 0.00001f ) 
			{
				return false;
			}
			m_started = true;
			m_value1 = _value1;
			m_value2 = _value2;
			m_time = _time;
			m_timing = 0.0f;
			m_invTime = 1.0f / m_time;
			m_prev = m_value1;
			return true;
		}

		void stop()
		{
			m_started = false;
		}

		bool update( float _timing, T* _out )
		{
			if( ( m_timing + _timing ) > m_time )
			{
				m_time = 0.0f;
				m_timing = 0.0f;
				*_out = m_value2;
				m_delta = (*_out) - m_prev;
				m_started = false;
				return true;
			}
			else
			{
				m_timing += _timing;
				*_out = ( m_value2 - m_value1 ) * m_invTime * m_timing + m_value1;
				m_delta = (*_out) - m_prev;
				m_prev = *_out;
				return false;
			}
			return false;
		}

		bool isStarted() const	{ return m_started; }

		const T& getDelta() const
		{
			return m_delta;
		}
	protected:
		T m_value1;
		T m_value2;
		T m_delta;
		T m_prev;
		float m_timing;
		float m_time;
		float m_invTime;
		bool m_started;
	};

}	// namespace Menge