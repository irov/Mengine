#	include "Animation.h"

#	include "ResourceManager.h"

#	include "ResourceAnimation.h"

#	include "BinParser.h"

#	include "LogEngine.h"

#	include "Math/rand.h"

#	include "pybind/system.hpp"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation()
	: m_resourceAnimation(0)
	, m_timinig(0)
	, m_currentFrame(0)
	, m_onEndFrameTick(false)
	, m_onEndFrameEvent(false)
	, m_onEndAnimationEvent(false)
	, m_animationFactor(1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Animation::~Animation()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::loader( BinParser * _parser )
	{
		Sprite::loader(_parser);

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Animation_Name, m_resourceAnimationName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimationResource( const ConstString& _resource )
	{
		if( m_resourceAnimationName == _resource ) 
		{
			return;
		}
		
		m_resourceAnimationName = _resource;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Animation::getAnimationResource() const
	{
		return m_resourceAnimationName;
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
	void Animation::_update( float _timing )
	{
		Sprite::_update( _timing );

		if( this->isPlay() == false )
		{
			return; 
		}

		std::size_t frameSize = m_resourceAnimation->getSequenceCount();

		m_timinig += _timing;

		float delay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
		delay *= m_animationFactor;

		while( m_timinig >= delay )
		{
			m_timinig -= delay;
			
			if( m_onEndFrameEvent == true )
			{
				this->callEvent( EVENT_FRAME_END, "(OI)", this->getEmbed(), m_currentFrame );
			}

			if( m_onEndFrameTick == true )
			{
				this->callEvent( EVENT_FRAME_TICK, "(OII)", this->getEmbed(), m_currentFrame, frameSize );
			}
			else
			{
				++m_currentFrame;
			}

			if( m_currentFrame == frameSize )
			{
				this->end();
			}	

			delay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
			delay *= m_animationFactor;
		}

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_activate()
	{
		if( Sprite::_activate() == false )
		{
			return false;
		}

		if( m_resourceAnimationName.empty() == true )
		{
			return false;
		}

		m_currentFrame = 0;

		if( m_resourceAnimation == NULL )
		{
			MENGE_LOG_ERROR( "Animation: '%s' Image resource not getting '%s'"
				, getName().c_str()
				, m_resourceAnimationName.c_str() 
				);

			return false;
		}

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_compile()
	{
		if( Sprite::_compile() == false )
		{
			return false;
		}

		if( m_resourceAnimationName.empty() == false )
		{	
			m_resourceAnimation = ResourceManager::get()
				->getResourceT<ResourceAnimation>( m_resourceAnimationName );

			if( m_resourceAnimation == 0 )
			{
				MENGE_LOG_ERROR( "Animation: '%s' no found resource with name '%s'"
					, m_name.c_str()
					, m_resourceAnimationName.c_str() 
					);

				return false;
			}

			m_currentFrame = 0;
			m_timinig = 0.0f;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_release()
	{
		Sprite::_release();

		ResourceManager::get()
			->releaseResource( m_resourceAnimation );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_play()
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' play not activate"
				, getName().c_str()
				);

			return false;
		}

		m_currentFrame = 0;

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
		this->setImageIndex( currentImageIndex );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_restart( std::size_t _enumerator )
	{
		//Todo
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_stop( std::size_t _enumerator )
	{
		m_currentFrame = 0;
		m_timinig = 0.f;

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex(m_currentFrame);
		this->setImageIndex( currentImageIndex );

		if( m_onEndAnimationEvent == true )
		{
			this->callEvent( EVENT_ANIMATION_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_end( std::size_t _enumerator )
	{
		m_currentFrame = 0;
		m_timinig = 0.f;

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex(m_currentFrame);
		this->setImageIndex( currentImageIndex );

		if( m_onEndAnimationEvent == true )
		{
			this->callEvent( EVENT_ANIMATION_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setEventListener( PyObject * _listener )
	{
		Sprite::_setEventListener( _listener );

		Eventable::registerEvent( EVENT_FRAME_END, ("onFrameEnd"), _listener, &m_onEndFrameEvent );
		Eventable::registerEvent( EVENT_FRAME_TICK, ("onFrameTick"), _listener, &m_onEndFrameTick );

		Eventable::registerEvent( EVENT_ANIMATION_END, ("onAnimationEnd"), _listener, &m_onEndAnimationEvent );
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Animation::getCurrentFrame() const
	{
		return m_currentFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Animation::getFrameCount() const
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' getFrameCount not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceAnimationName.c_str()
				);

			return 0;
		}

		return m_resourceAnimation->getSequenceCount();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getFrameDelay( std::size_t  _frame ) const
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' getFrameDelay not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceAnimationName.c_str()
				);

			return 0;
		}

		return m_resourceAnimation->getSequenceDelay( _frame );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setCurrentFrame( std::size_t _frame )
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' setCurrentFrame not activate"
				, m_name.c_str()
				);

			return;
		}

		std::size_t sequenceCount = m_resourceAnimation->getSequenceCount();

		if( _frame >= sequenceCount )	
		{
			MENGE_LOG_ERROR( "Animation: '%s' setCurrentFrame _frame(%d) >= sequenceCount(%d)"
				, m_name.c_str()
				, _frame
				, sequenceCount
				);

			return;
		}

		m_currentFrame = _frame;

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
		this->setImageIndex( currentImageIndex );
	}
}
