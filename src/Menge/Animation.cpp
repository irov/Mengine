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
	: m_resource(0)
	, m_playing(false)
	, m_autoStart(false)
	, m_looping(false)
	, m_delay(0)
	, m_currentFrame(0)
	//, m_listener(0)
	, m_animationFactor(1.f)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Animation::loader( TiXmlElement * _xml )
	{
		Sprite::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "Animation", "Name", m_resourcename );
			XML_CHECK_VALUE_NODE( "Looping", "Value", m_looping );
			XML_CHECK_VALUE_NODE( "AutoStart", "Value", m_autoStart );			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimationListener( PyObject * _listener )
	{
		m_listener = _listener;

		registerEventListener("END_ANIMATION", "onAnimationEnd", m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimationResource( const std::string & _resource )
	{
		m_resourcename = _resource;

		if( m_resource )
		{
			Holder<ResourceManager>::hostage()
				->releaseResource( m_resource );
		}

		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceAnimation>( m_resourcename );		
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimationFactor( float _factor )
	{
		m_animationFactor = _factor;
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

		m_delay += _timing;

		float delay = m_resource->getSequenceDelay( m_currentFrame );
		delay *= m_animationFactor;

		while( m_delay >= delay )
		{
			m_delay -= delay;
			
			size_t frameSize = m_resource->getSequenceCount();

			if( ++m_currentFrame == frameSize )
			{
				if( m_looping == false )
				{
					m_playing = false;
					--m_currentFrame;

					callEvent( "END_ANIMATION", "(O)", this->getScript() );
					return;
				}
				else
				{
					m_currentFrame = 0;
				}
			}	
			delay = m_resource->getSequenceDelay( m_currentFrame );
			delay *= m_animationFactor;
		}

		size_t currentImageIndex = m_resource->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );

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

			size_t currentImageIndex = m_resource->getSequenceIndex( m_currentFrame );
			setImageIndex( currentImageIndex );
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
		m_delay = 0;

		if( m_playing == true )
		{
			m_playing = false;
			callEvent( "END_ANIMATION", "(O)", this->getScript() );
		}

		size_t currentImageIndex = m_resource->getSequenceIndex(m_currentFrame);
		setImageIndex( currentImageIndex );
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
		m_delay = 0;
		m_currentFrame = 0;

		size_t currentImageIndex = m_resource->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
}