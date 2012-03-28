#	include "Animation.h"

#	include "ResourceManager.h"

#	include "ResourceAnimation.h"

#	include "ResourceImage.h"

#	include "BinParser.h"

#	include "LogEngine.h"

#	include "Math/rand.h"

#	include "pybind/system.hpp"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation()
		: m_resourceAnimation(0)
		, m_timing(0)
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
			BIN_CASE_ATTRIBUTE( Protocol::ImageIndex_Value, m_currentFrame );
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

		this->recompile();
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
		if( this->isPlay() == false )
		{
			return; 
		}

		size_t frameSize = m_resourceAnimation->getSequenceCount();

		m_timing += _timing;

		float delay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
		delay *= m_animationFactor;

		while( m_timing >= delay )
		{
			m_timing -= delay;

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
				if( this->getLoop() == true )
				{
					m_currentFrame = 0;
				}
				else
				{
					this->end();
					return;
				}
			}	

			delay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
			delay *= m_animationFactor;
		}

		this->updateCurrentImageResource_();

		Sprite::_update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_activate()
	{
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

		if( this->updateCurrentImageResource_() == false )
		{
			return false;
		}
			
		if( Sprite::_activate() == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_compile()
	{
		if( m_resourceAnimationName.empty() == true )
		{	
			MENGE_LOG_ERROR( "Animation: '%s' no set resource name"
				, m_name.c_str()
				);

			return false;
		}
		
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
		m_timing = 0.0f;
		
		if( this->updateCurrentImageResource_() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_release()
	{
		Sprite::_release();

		if( m_resourceAnimation != 0 )
		{
			m_resourceAnimation->decrementReference();
			m_resourceAnimation = 0;
		}

		m_play = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_play()
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' play not activate"
				, getName().c_str()
				);

			return false;
		}
		
		m_currentFrame = 0; 
		m_timing = 0.f;
	
		if( this->updateCurrentImageResource_() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_restart( size_t _enumerator )
	{
		if( this->isActivate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_stop( size_t _enumerator )
	{
			if( this->isActivate() == false )
			{
				MENGE_LOG_ERROR( "Animation: '%s' stop not activate"
					, getName().c_str()
					);

				return;
			}

			m_currentFrame = 0;
			m_timing = 0.f;

			if( this->updateCurrentImageResource_() == false )
			{
				return;
			}

			if( m_onEndAnimationEvent == true )
			{
				this->callEvent( EVENT_ANIMATION_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
			}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_end( size_t _enumerator )
	{
			if( this->isActivate() == false )
			{
				MENGE_LOG_ERROR( "Animation: '%s' end not activate"
					, getName().c_str()
					);

				return;
			}

			m_timing = 0.f;
			//m_currentFrame = m_resourceAnimation->getLastFrameIndex();
			m_currentFrame = 0;

			if( this->updateCurrentImageResource_() == false )
			{
				return;
			}

			if( m_onEndAnimationEvent == true )
			{
				this->callEvent( EVENT_ANIMATION_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
			}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::updateCurrentImageResource_()
	{
		//const ConstString& currentResourceName = m_resourceAnimation->getSequenceResourceName( m_currentFrame );
		//setImageResource( currentResourceName );
		m_resource = NULL;

		m_resourceName =  m_resourceAnimation->getSequenceResourceName( m_currentFrame );

		if( Sprite::_compile() == false )
		{
			return false;
		}

		return true;
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
	size_t Animation::getCurrentFrame() const
	{
		return m_currentFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Animation::getFrameCount() const
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation.getFrameCount: '%s' not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceAnimationName.c_str()
				);

			return 0;
		}

		return m_resourceAnimation->getSequenceCount();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getFrameDelay( size_t  _frame ) const
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation.getFrameDelay: '%s' not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceAnimationName.c_str()
				);

			return 0;
		}

		return m_resourceAnimation->getSequenceDelay( _frame );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setCurrentFrame( size_t _frame )
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation.setCurrentFrame: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		size_t sequenceCount = m_resourceAnimation->getSequenceCount();

		if( _frame >= sequenceCount )	
		{
			MENGE_LOG_ERROR( "Animation.setCurrentFrame: '%s' _frame(%d) <> sequenceCount(%d)"
				, m_name.c_str()
				, _frame
				, sequenceCount
				);

			return;
		}

		m_currentFrame = _frame;

		this->updateCurrentImageResource_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setTiming( float _timing )
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' setTiming not activate"
				, m_name.c_str()
				);

			return;
		}
				
		m_timing = _timing;
		this->updateCurrentImageResource_();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getTiming() const
	{
		return m_timing;
	}
	//////////////////////////////////////////////////////////////////////////
}
