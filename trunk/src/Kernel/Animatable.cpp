#	include "Kernel/Animatable.h"

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
        , m_scretch(1.f)
		, m_play(false)
		, m_interrupt(false)
		, m_loop(false)
		, m_reverse(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setLoop( bool _value )
	{
		m_loop = _value;

        this->_setLoop( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setAnimationSpeedFactor( float _factor )
	{
		m_animationSpeedFactor = _factor;

		this->_setSpeedFactor( _factor );
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
        if( this->getReverse() == false )
        {
            this->_setLastFrame();            
        }
        else
        {
            this->_setFirstFrame();
        }		
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setLastFrame()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setFirstFrame()
	{		
        if( this->getReverse() == false )
        {
		    this->_setFirstFrame();
        }
        else
        {
            this->_setLastFrame();
        }
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
	void Animatable::setReverse( bool _value)
	{
		m_reverse = _value;

		this->_setReverse( _value );
	}
    //////////////////////////////////////////////////////////////////////////
    void Animatable::setPlayCount( uint32_t _count )
    {
        m_playCount = _count;
    }
	//////////////////////////////////////////////////////////////////////////
	void Animatable::_setReverse( bool _value )
	{
        (void)_value;
		//Empty
	}
    //////////////////////////////////////////////////////////////////////////
    void Animatable::setScretch( float _scretch )
    {
        m_scretch = _scretch;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t Animatable::play( float _time )
	{
		m_interrupt = false;

		m_playTime = _time;		

		if( m_play == true )
		{
			if( this->_restart( _time, m_enumerator ) == false )
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
		m_interrupt = false;

		this->_stop( m_enumerator );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animatable::interrupt()
	{
		if( m_play == false )
		{
			return false;
		}

		//m_play = false;
		m_interrupt = true;

		bool result = this->_interrupt( m_enumerator );

		return result;
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
	void Animatable::_setSpeedFactor( float _factor )
	{
        (void)_factor;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setIntervalStart( float _startInterval )
	{
		m_intervalStart = _startInterval;
	}
    //////////////////////////////////////////////////////////////////////////
    void Animatable::setInterval( float _begin, float _end )
    {
        m_intervalBegin = _begin;
        m_intervalEnd = _end;
    }
	//////////////////////////////////////////////////////////////////////////
}