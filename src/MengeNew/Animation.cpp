#	include "Animation.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"

#	include "ResourceAnimation.h"

#	include "XmlEngine.h"

namespace	Menge
{
	OBJECT_IMPLEMENT(Animation)
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation()
	: m_resource(0)
	, m_playing(false)
	, m_autoStart(false)
	, m_looping(false)
	, m_delayAnimation(0.f)
	, m_delayFrame(0.f)
	, m_currentFrame(0)
	, m_countFrames(0)
	, m_onEndFrameEvent(false)
	, m_onEndAnimationEvent(false)
	, m_animationFactor(1.f)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Animation::loader( XmlElement * _xml )
	{
		Sprite::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Animation", "Name", m_resourcename );
			XML_CASE_ATTRIBUTE_NODE( "Looping", "Value", m_looping );
			XML_CASE_ATTRIBUTE_NODE( "AutoStart", "Value", m_autoStart );			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimationResource( const std::string & _resource )
	{
		m_resourcename = _resource;
		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimationFactor( float _factor )
	{
		m_animationFactor = _factor;

		updateImageData_();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getAnimationFactor() const
	{
		return m_animationFactor;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setLooped( bool _loop )
	{
		m_looping = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::getLooped() const
	{
		return m_looping;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_update( float _timing )
	{
		Sprite::_update( _timing );

		if( m_playing == false )
		{
			return; 
		}

		m_delayAnimation += _timing;

		while( m_delayAnimation >= m_delayFrame )
		{
			m_delayAnimation -= m_delayFrame;
			
			if( m_onEndFrameEvent )
			{
				callEvent( "END_FRAME", "(OI)", this->getEmbedding(), m_currentFrame );
			}

			if( ++m_currentFrame == m_countFrames )
			{
				if( m_looping == false )
				{
					m_playing = false;
					--m_currentFrame;

					if( m_onEndAnimationEvent )
					{
						callEvent( "END_ANIMATION", "(O)", this->getEmbedding() );
					}

					return;
				}
				else
				{
					m_currentFrame = 0;
				}
			}	

			updateImageData_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_activate()
	{
		if( Sprite::_activate() == false )
		{
			return false;
		}

		if( m_autoStart )
		{
			play_();
		}
		else
		{
			m_currentFrame = 0;

			updateImageData_();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_deactivate()
	{
		Sprite::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_compile()
	{
		if( Sprite::_compile() == false )
		{
			return false;
		}

		m_resource = Holder<ResourceManager>::hostage()
			->getResourceT<ResourceAnimation>( m_resourcename );

		if( m_resource == 0 )
		{
			return false;
		}

		m_countFrames = m_resource->getSequenceCount();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_release()
	{
		Sprite::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::stop()
	{
		m_currentFrame = 0;
		m_delayAnimation = 0;

		if( m_playing == true )
		{
			m_playing = false;
			
			if( m_onEndAnimationEvent )
			{
				callEvent( "END_ANIMATION", "(O)", this->getEmbedding() );
			}
		}

		updateImageData_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::pause()
	{
		m_playing = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::play()
	{
		if( isActivate() == false )
		{
			return;
		}

		play_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::play_()
	{
		m_playing = true;
		m_delayAnimation = 0.f;
		m_currentFrame = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::updateImageData_()
	{
		m_delayFrame = m_resource->getSequenceDelay( m_currentFrame );
		m_delayFrame *= m_animationFactor;

		unsigned int currentImageIndex = m_resource->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setListener()
	{
		Sprite::_setListener();

		m_onEndAnimationEvent = registerEvent("END_ANIMATION", "onAnimationEnd", m_listener );
		m_onEndFrameEvent = registerEvent("END_FRAME", "onFrameEnd", m_listener );
	}

}