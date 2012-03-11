#	pragma once

namespace Menge
{
	template <typename T>
	class ValueAccumulator
	{
	public:
		ValueAccumulator()
			: m_started(false)
			, m_timing(0.f)
		{
		}

	public:
		void stop()
		{
			m_started = false;
		}

		bool isStarted() const	
		{ 
			return m_started; 
		}
		
		virtual bool update( float _timing, T * _out ) = 0;

	protected:
		bool m_started;
		float m_timing;

		T m_pos;
		T m_delta;			
	};

	template <typename T>
	class ValueInterpolator
	{
	public:
		ValueInterpolator()
			: m_started(false)
			, m_timing(0.f)
			, m_time(0.f)
		{
		}
		
	public:
		void stop()
		{
			m_started = false;
		}

		bool isStarted() const	
		{ 
			return m_started; 
		}

		const T & getValue() const
		{
			return m_prev;
		}

		virtual bool update( float _timing, T * _out ) = 0;

	protected:
		bool m_started;
		float m_timing;

		T m_value1;
		T m_value2;
		T m_prev;
		T m_delta;
		float m_time;
	};

	template <typename T>
	class ValueAccumulateLinear
		: public ValueAccumulator<T>
	{
	public:
		template<class ABS>
		bool start( const T & _pos, const T & _acc, float _speed, ABS _abs )
		{
			ValueAccumulator<T>::m_started = false;

			ValueAccumulator<T>::m_pos = _pos;
			ValueAccumulator<T>::m_delta = _acc * _speed;
			ValueAccumulator<T>::m_timing = 0.f;

			if( _speed < 0.00001f || _abs( _acc ) < 0.00001f ) 
			{
				return false;
			}

			ValueAccumulator<T>::m_started = true;

			return true;
		}

		bool update( float _timing, T * _out )
		{
			if( ValueAccumulator<T>::m_started == false )
			{
				*_out = ValueAccumulator<T>::m_pos;
				return true;
			}

			ValueAccumulator<T>::m_timing += _timing;

			ValueAccumulator<T>::m_pos += ValueAccumulator<T>::m_delta * _timing;

			*_out = ValueAccumulator<T>::m_pos;

			return false;
		}
	};

	template <typename T>
	class ValueInterpolatorLinear
		: public ValueInterpolator<T>
	{
	public:
		template<class ABS>
		bool start( const T& _value1, const T& _value2, float _time, ABS _abs )
		{
			ValueInterpolator<T>::m_started = false;

			ValueInterpolator<T>::m_value1 = _value1;
			ValueInterpolator<T>::m_value2 = _value2;
			ValueInterpolator<T>::m_prev = ValueInterpolator<T>::m_value1;
			ValueInterpolator<T>::m_delta = ValueInterpolator<T>::m_prev - ValueInterpolator<T>::m_value1;
			ValueInterpolator<T>::m_time = _time * 1000.f;
			ValueInterpolator<T>::m_timing = 0.f;
			if( _time < 0.00001f || _abs( _value2 - _value1 ) < 0.00001f ) 
			{
				return false;
			}

			ValueInterpolator<T>::m_started = true;
			m_invTime = 1.0f / ValueInterpolator<T>::m_time;
			return true;
		}


		bool update( float _timing, T * _out )
		{
			if( ValueInterpolator<T>::m_started == false )
			{
				*_out = ValueInterpolator<T>::m_value2;
				return true;
			}

			ValueInterpolator<T>::m_timing += _timing;

			if( ValueInterpolator<T>::m_timing > ValueInterpolator<T>::m_time )
			{
				ValueInterpolator<T>::m_time = 0.f;
				ValueInterpolator<T>::m_timing = 0.f;
				*_out = ValueInterpolator<T>::m_value2;
				ValueInterpolator<T>::m_delta = (*_out) - ValueInterpolator<T>::m_prev;
				ValueInterpolator<T>::m_started = false;

				return true;
			}
						
			*_out = ( ValueInterpolator<T>::m_value2 - ValueInterpolator<T>::m_value1 ) * m_invTime * ValueInterpolator<T>::m_timing + ValueInterpolator<T>::m_value1;
			ValueInterpolator<T>::m_delta = (*_out) - ValueInterpolator<T>::m_prev;
			ValueInterpolator<T>::m_prev = *_out;
			
			return false;
		}


	protected:
		float m_invTime;
	};

	template <typename T>
	class ValueInterpolatorQuadratic
		: public ValueInterpolator<T>
	{
	public:

		template<class ABS>
		bool start( const T& _value1, const T& _value2, const T& _v0, float _time, ABS _abs )
		{
			ValueInterpolator<T>::m_started = false;

			ValueInterpolator<T>::m_value1 = _value1;
			ValueInterpolator<T>::m_value2 = _value2;
			ValueInterpolator<T>::m_prev = ValueInterpolator<T>::m_value1;
			ValueInterpolator<T>::m_delta = ValueInterpolator<T>::m_prev - ValueInterpolator<T>::m_value1;
			ValueInterpolator<T>::m_time = _time * 1000.f;
			ValueInterpolator<T>::m_timing = 0.f;
			
			m_v0 = _v0;
			if( _time < 0.00001f || _abs( _value2 - _value1 ) < 0.00001f ) 
			{
				return false;
			}

			ValueInterpolator<T>::m_started = true;
			float invTime = 1.0f / ValueInterpolator<T>::m_time;
			m_a = ( ValueInterpolator<T>::m_value2 - ValueInterpolator<T>::m_value1 - m_v0 * ValueInterpolator<T>::m_time ) * 2.0f * invTime * invTime;

			return true;
		}


		bool update( float _timing, T * _out )
		{
			if( ValueInterpolator<T>::m_started == false )
			{
				*_out = ValueInterpolator<T>::m_value2;
				return true;
			}
			if( ( ValueInterpolator<T>::m_timing + _timing ) > ValueInterpolator<T>::m_time )
			{
				ValueInterpolator<T>::m_time = 0.f;
				ValueInterpolator<T>::m_timing = 0.f;
				*_out = ValueInterpolator<T>::m_value2;
				ValueInterpolator<T>::m_delta = (*_out) - ValueInterpolator<T>::m_prev;
				ValueInterpolator<T>::m_started = false;
				return true;
			}
			else
			{
				ValueInterpolator<T>::m_timing += _timing;
				*_out = ValueInterpolator<T>::m_value1 + m_v0 * ValueInterpolator<T>::m_timing + m_a * 0.5f * ValueInterpolator<T>::m_timing * ValueInterpolator<T>::m_timing;
				ValueInterpolator<T>::m_delta = (*_out) - ValueInterpolator<T>::m_prev;
				ValueInterpolator<T>::m_prev = *_out;
				return false;
			}

			return false;
		}
	protected:
		T m_v0;
		T m_a;
	};

	template <typename T>
	class ValueInterpolatorQuadraticBezier
		: public ValueInterpolator<T>
	{
	public:
		template<class ABS>
		bool start( const T& _value1, const T& _value2, const T& _v0, float _time, ABS _abs )
		{
			ValueInterpolator<T>::m_started = false;

			ValueInterpolator<T>::m_value1 = _value1;
			ValueInterpolator<T>::m_value2 = _value2;
			ValueInterpolator<T>::m_prev = ValueInterpolator<T>::m_value1;
			ValueInterpolator<T>::m_delta = ValueInterpolator<T>::m_prev - ValueInterpolator<T>::m_value1;
			ValueInterpolator<T>::m_time = _time * 1000.f;
			ValueInterpolator<T>::m_timing = 0.f;

			m_v0 = _v0;

			if( _time < 0.00001f || _abs( _value2 - _value1 ) < 0.00001f ) 
			{
				return false;
			}

			ValueInterpolator<T>::m_started = true;
			return true;
		}


		bool update( float _timing, T * _out )
		{
			if( ValueInterpolator<T>::m_started == false )
			{
				*_out = ValueInterpolator<T>::m_value2;
				return true;
			}
			if( ( ValueInterpolator<T>::m_timing + _timing ) > ValueInterpolator<T>::m_time )
			{
				ValueInterpolator<T>::m_time = 0.f;
				ValueInterpolator<T>::m_timing = 0.f;
				*_out = ValueInterpolator<T>::m_value2;
				ValueInterpolator<T>::m_delta = (*_out) - ValueInterpolator<T>::m_prev;
				ValueInterpolator<T>::m_started = false;
				return true;
			}
			else
			{
				ValueInterpolator<T>::m_timing += _timing;
				float t_time = ValueInterpolator<T>::m_timing / ValueInterpolator<T>::m_time;
				*_out = (1-t_time)*(1-t_time)*ValueInterpolator<T>::m_value1
					+ 2*t_time*(1-t_time)*m_v0
					+ t_time*t_time*ValueInterpolator<T>::m_value2;
				ValueInterpolator<T>::m_delta = (*_out) - ValueInterpolator<T>::m_prev;
				ValueInterpolator<T>::m_prev = *_out;
				return false;
			}

			return false;
		}
	protected:
		T m_v0;
	};

	template <typename T>
	class ValueInterpolatorCubicBezier
		: public ValueInterpolator<T>
	{
	public:
		template<class ABS>
		bool start( const T& _value1, const T& _value2, const T& _v0, const T& _v1, float _time, ABS _abs )
		{
			ValueInterpolator<T>::m_started = false;

			ValueInterpolator<T>::m_value1 = _value1;
			ValueInterpolator<T>::m_value2 = _value2;
			ValueInterpolator<T>::m_prev = ValueInterpolator<T>::m_value1;
			ValueInterpolator<T>::m_delta = ValueInterpolator<T>::m_prev - ValueInterpolator<T>::m_value1;
			ValueInterpolator<T>::m_time = _time * 1000.f;
			ValueInterpolator<T>::m_timing = 0.f;

			m_v0 = _v0;
			m_v1 = _v1;

			if( _time < 0.00001f || _abs( _value2 - _value1 ) < 0.00001f ) 
			{
				return false;
			}

			ValueInterpolator<T>::m_started = true;
			return true;
		}


		bool update( float _timing, T * _out )
		{
			if( ValueInterpolator<T>::m_started == false )
			{
				*_out = ValueInterpolator<T>::m_value2;
				return true;
			}
			if( ( ValueInterpolator<T>::m_timing + _timing ) > ValueInterpolator<T>::m_time )
			{
				ValueInterpolator<T>::m_time = 0.f;
				ValueInterpolator<T>::m_timing = 0.f;
				*_out = ValueInterpolator<T>::m_value2;
				ValueInterpolator<T>::m_delta = (*_out) - ValueInterpolator<T>::m_prev;
				ValueInterpolator<T>::m_started = false;
				return true;
			}
			else
			{
				ValueInterpolator<T>::m_timing += _timing;
				float t_time = ValueInterpolator<T>::m_timing / ValueInterpolator<T>::m_time;
				*_out = (1-t_time)*(1-t_time)*(1-t_time)*ValueInterpolator<T>::m_value1
					+ 3*t_time*(1-t_time)*(1-t_time)*m_v0
					+ 3*t_time*t_time*(1-t_time)*m_v1
					+ t_time*t_time*t_time*ValueInterpolator<T>::m_value2;
				ValueInterpolator<T>::m_delta = (*_out) - ValueInterpolator<T>::m_prev;
				ValueInterpolator<T>::m_prev = *_out;
				return false;
			}

			return false;
		}
	protected:
		T m_v0;
		T m_v1;
	};
}
