#	include "Animatable.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Animatable::Animatable()
		: m_enumerator(0)
		, m_play(false)
		, m_loop(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Animatable::setLoop( bool _value )
	{
		m_loop = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animatable::getLoop() const
	{
		return m_loop;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Animatable::play()
	{
		if( m_play == true )
		{
			if( this->_restart( m_enumerator ) == false )
			{
				return 0;
			}
		}
		else
		{
			if( this->_play() == false )
			{
				return 0;
			}
		}

		m_play = true;

		std::size_t id = ++m_enumerator;

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animatable::stop()
	{
		if( m_play == false )
		{
			return false;
		}

		m_play = false;

		this->_stop( m_enumerator );

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

		this->_end( m_enumerator );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animatable::isPlay() const
	{
		return m_play;
	}
}