#	include "Animation.h"
#	include "ObjectImplement.h"

#	include "ResourceImage.h"

OBJECT_IMPLEMENT(Animation)

Animation::Animation()
: m_playing(false)
, m_looping(true)
, m_state(FORWARD)
, m_total_delay(0.f)
{

}
//////////////////////////////////////////////////////////////////////////
void Animation::setLooped(bool _looped)
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
	assert(m_state == FORWARD || m_state == REWIND);

	size_t frameSize = m_image->getCount();

	m_currentFrame = 
		(m_state == FORWARD)
		? 0
		: frameSize - 1;
}
//////////////////////////////////////////////////////////////////////////
void Animation::nextFrame()
{
	size_t frimeSize = m_image->getCount();

	if( ++m_currentFrame == frimeSize )
	{
		if( m_looping == false )
		{
			m_playing = false;
			m_currentFrame = frimeSize - 1;
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
	size_t frimeSize = m_image->getCount();

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
			m_currentFrame = frimeSize;
		}
	}
	--m_currentFrame;
}
//////////////////////////////////////////////////////////////////////////
void Animation::_update(float _timing)
{
	//assert(m_state == FORWARD || m_state == REWIND);

	//if(!m_playing)
	//{
	//	return; 
	//}

	//m_total_delay += _timing;

	//int delay = m_image->getFrameDelay( m_currentFrame );

	//while(m_total_delay >= delay)
	//{
	//	m_total_delay -= delay;

	//	switch(m_state)
	//	{
	//	case FORWARD:
	//		{
	//			nextFrame();
	//		}
	//		break;

	//	case REWIND:
	//		{
	//			prevFrame();
	//		}
	//		break;

	//	default:
	//		{
	//			assert(!"undefined state!");
	//		}
	//		break;
	//	}
	//	delay = m_image->getFrameDelay( m_currentFrame );
	//}
}
//////////////////////////////////////////////////////////////////////////
bool Animation::_activate()
{
	if( Sprite::_activate() == false )
	{
		return false;
	}

	setFirstFrame();

	return true;
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