#	include "Animatable.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Animatable::Animatable()
		: m_enumerator(0)
		, m_play(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Animatable::play()
	{
		if( m_play == true )
		{
			this->_restart( m_enumerator );
		}

		if( this->_play() == false )
		{
			return 0;
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