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
	, m_autoStart(false)
	, m_looping(false)
	, m_total_delay(0)
	, m_animation(0)
	, m_currentFrame(0)
	, m_listener(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Animation::loader( TiXmlElement * _xml )
	{
		Sprite::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "Animation", "Name", m_resourceAnimation );
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
		m_resourceAnimation = _resource;

		if( m_animation )
		{
			Holder<ResourceManager>::hostage()
				->releaseResource( m_animation );
		}

		m_animation = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceAnimation>( m_resourceAnimation );		
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
	void Animation::_update( size_t _timing, const Viewport & _viewport )
	{
		Sprite::_update( _timing, _viewport );

		if( m_playing == false )
		{
			return; 
		}

		m_total_delay += _timing;

		size_t delay = m_animation->getSequenceDelay( m_currentFrame );

		while( m_total_delay >= delay )
		{
			m_total_delay -= delay;
			
			size_t frameSize = m_animation->getSequenceCount();

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
			delay = m_animation->getSequenceDelay( m_currentFrame );
		}

		size_t currentImageIndex = m_animation->getSequenceIndex(m_currentFrame);
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
			play();
		}
		else
		{
			m_currentFrame = 0;

			size_t currentImageIndex = m_animation->getSequenceIndex(m_currentFrame);
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

		m_animation = Holder<ResourceManager>::hostage()
			->getResourceT<ResourceAnimation>( m_resourceAnimation );

		if( m_animation == 0 )
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
			->releaseResource( m_animation );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::stop()
	{
		m_currentFrame = 0;
		m_total_delay = 0;

		if( m_playing == true )
		{
			m_playing = false;
			callEvent( "END_ANIMATION", "(O)", this->getScript() );
		}

		size_t currentImageIndex = m_animation->getSequenceIndex(m_currentFrame);
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
		if( isActive() == false )
		{
			return;
		}

		m_playing = true;
		m_total_delay = 0;
		m_currentFrame = 0;

		size_t currentImageIndex = m_animation->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
}