#	include "Animation.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"

#	include "ResourceAnimation.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	OBJECT_IMPLEMENT(Animation)
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation()
	: m_playing(false)
	, m_looping(false)
	, m_state(FORWARD)
	, m_total_delay(0.f)
	, m_anim(0)
	, m_currentFrame(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Animation::loader( TiXmlElement * _xml )
	{
		Sprite::loader(_xml);
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "Animation", "Name", m_resourceAnim );
			XML_CHECK_VALUE_NODE( "Looping", "Value", m_looping );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimState( eAnimState _state )
	{
		m_state = _state;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setLooped( bool _looped )
	{
		m_looping = _looped;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::getLooped() const
	{
		return m_looping;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setFirstFrame()
	{
		size_t frameSize = m_anim->getSequenceCount();

		m_currentFrame = (m_state == FORWARD) ? 0 : frameSize - 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::nextFrame()
	{
		size_t frameSize = m_anim->getSequenceCount();

		if( ++m_currentFrame == frameSize )
		{
			if( m_looping == false )
			{
				m_playing = false;
				m_currentFrame = frameSize - 1;
				return;
			}
			else
			{
				m_currentFrame = 0;
			}
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::prevFrame()
	{
		if( m_currentFrame == 0 )
		{
			if(!m_looping)
			{
				m_playing = false;
				m_currentFrame = 0;
				return;
			}
			else
			{
				m_currentFrame = m_anim->getSequenceCount();
			}
		}
		--m_currentFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_update(float _timing)
	{
		Sprite::_update( _timing );

		if( m_playing == false )
		{
			return; 
		}

		m_total_delay += _timing;

		int delay = m_anim->getSequenceDelay(m_currentFrame);

		size_t currentImageIndex = m_anim->getSequenceIndex(m_currentFrame);

		setImageIndex( currentImageIndex );

		while( m_total_delay >= delay )
		{
			m_total_delay -= delay;

			switch(m_state)
			{
			case FORWARD:
				{
					nextFrame();
				}
				break;

			case REWIND:
				{
					prevFrame();
				}
				break;

			default:
				{
					assert(!"undefined state!");
				}
				break;
			}
			delay = m_anim->getSequenceDelay( m_currentFrame );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_activate()
	{
		if( Sprite::_activate() == false )
		{
			return false;
		}

		m_anim = Holder<ResourceManager>::hostage()
			->getResourceT<ResourceAnimation>( m_resourceAnim );

		if( m_anim == NULL )
		{
			return false;
		}

		setFirstFrame();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_deactivate()
	{
		Sprite::_deactivate();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_anim );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::stop()
	{
		m_playing = false;
		setFirstFrame();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::pause()
	{
		m_playing = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::play()
	{
		m_playing = true;
		setFirstFrame();
	}
	//////////////////////////////////////////////////////////////////////////
}