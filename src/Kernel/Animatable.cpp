#	include "Kernel/Animatable.h"

#	include "Math/utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Animatable::Animatable()
		: m_enumerator(0)
		, m_playTime(0.f)
		, m_animationSpeedFactor(1.f)
		, m_intervalStart(0.f)
        , m_intervalBegin(0.f)
        , m_intervalEnd(-1.f)
        , m_playCount(1)
		, m_playIterator(0)
        , m_stretch(1.f)
		, m_autoPlay(false)
		, m_play(false)
		, m_pause(false)
		, m_interrupt(false)
		, m_loop(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setLoop( bool _value )
	{
		if( m_loop == _value )
		{
			return;
		}

		m_loop = _value;

        this->_setLoop( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setAnimationSpeedFactor( float _factor )
	{
		if( mt::equal_f_f( m_animationSpeedFactor, _factor ) == true )
		{
			return;
		}

		m_animationSpeedFactor = _factor;

		this->_setAnimationSpeedFactor( _factor );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setTiming( float _timing )
	{		
		float timing = m_intervalStart + m_intervalBegin + _timing;

		this->_setTiming( timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setTiming( float _timing )
	{
        (void)_timing;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setLastFrame()
	{		
        this->_setLastFrame();            	
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setLastFrame()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setFirstFrame()
	{		
		this->_setFirstFrame();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setFirstFrame()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	float Animatable::getTiming() const
	{
		float timing = this->_getTiming();

		return timing;
	}
	//////////////////////////////////////////////////////////////////////////
	float Animatable::_getTiming() const
	{
		return 0.f;
	}
    //////////////////////////////////////////////////////////////////////////
    void Animatable::setPlayCount( uint32_t _count )
    {
		if( m_playCount == _count )
		{
			return;
		}

        m_playCount = _count;

		this->_setPlayCount( _count );
    }
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setPlayCount( uint32_t _count )
	{
		(void)_count;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setAutoPlay( bool _autoPlay )
	{
		if( m_autoPlay == _autoPlay )
		{
			return;
		}

		m_autoPlay = _autoPlay;

		this->_setAutoPlay( _autoPlay );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setAutoPlay( bool _autoPlay )
	{
		(void)_autoPlay;
		//Empty
	}
    //////////////////////////////////////////////////////////////////////////
    void Animatable::setStretch( float _scretch )
    {
		if( mt::equal_f_f( m_stretch, _scretch ) == true )
		{
			return;
		}

        m_stretch = _scretch;

		this->_setStretch( _scretch );
    }
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setStretch( float _scretch )
	{
		(void)_scretch;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Animatable::play( float _time )
	{
		if( m_pause == true )
		{
			return 0;
		}
		
		m_playIterator = m_playCount;

		if( m_play == true )
		{
			if( this->_restart( m_enumerator, _time ) == false )
			{
				return 0;
			}
		}
		else
		{
			if( this->_play( _time ) == false )
			{
				return 0;
			}
		}

		m_interrupt = false;

		m_playTime = _time;

		m_play = true;

		uint32_t id = ++m_enumerator;

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animatable::stop()
	{
		if( m_play == false )
		{
			return false;
		}

		m_playTime = 0.f;
		m_play = false;
		m_pause = false;
		m_interrupt = false;

		this->_stop( m_enumerator );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animatable::pause()
	{
		if( m_play == false )
		{
			return false;
		}

		m_pause = true;

		this->_pause( m_enumerator );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::resume( float _time )
	{
		if( m_play == false )
		{
			return;
		}

		if( m_pause == false )
		{
			return;
		}

		m_pause = false;

		this->_resume( m_enumerator, _time );

		m_playTime = _time;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animatable::interrupt()
	{
		if( m_play == false )
		{
			return false;
		}

		if( m_interrupt == true )
		{
			return true;
		}

		m_interrupt = true;

		if( this->_interrupt( m_enumerator ) == false )
		{
			m_interrupt = false;

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::end()
	{
		if( m_play == false )
		{
			return;
		}

		m_play = false;
		m_interrupt = false;

		this->_end( m_enumerator );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setLoop( bool _value )
	{
        (void)_value;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setAnimationSpeedFactor( float _factor )
	{
        (void)_factor;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setIntervalStart( float _startInterval )
	{
		if( mt::equal_f_f( m_intervalStart, _startInterval ) == true )
		{
			return;
		}

		m_intervalStart = _startInterval;

		this->_setIntervalStart( _startInterval );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setIntervalStart( float _startInterval )
	{
		(void)_startInterval;
		//Empty
	}
    //////////////////////////////////////////////////////////////////////////
    void Animatable::setInterval( float _begin, float _end )
    {
		if( mt::equal_f_f( m_intervalBegin, _begin ) == true && mt::equal_f_f( m_intervalEnd, _end ) == true )
		{
			return;
		}

        m_intervalBegin = _begin;
        m_intervalEnd = _end;

		this->_setInterval( _begin, _end );
    }
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setInterval( float _begin, float _end )
	{
		(void)_begin;
		(void)_end;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
}