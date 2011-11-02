#	include "Animation.h"

#	include "ResourceManager.h"

#	include "ResourceSequence.h"

#	include "BinParser.h"

#	include "LogEngine.h"

#	include "Math/rand.h"

#	include "pybind/system.hpp"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation()
	: m_resourceSequence(0)
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
			BIN_CASE_ATTRIBUTE( Protocol::Animation_Name, m_resourceSequenceName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setSequenceResource( const ConstString& _resource )
	{
		if( m_resourceSequenceName == _resource ) 
		{
			return;
		}
		
		m_resourceSequenceName = _resource;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Animation::getSequenceResource() const
	{
		return m_resourceSequenceName;
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

		std::size_t frameSize = m_resourceSequence->getSequenceCount();

		m_timinig += _timing;

		float delay = m_resourceSequence->getSequenceDelay( m_currentFrame );
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
				if( this->getLoop() == true )
				{
					m_currentFrame = 0;
				}
				else
				{
					this->end();					
				}
			}	

			delay = m_resourceSequence->getSequenceDelay( m_currentFrame );
			delay *= m_animationFactor;
		}

		std::size_t currentImageIndex = m_resourceSequence->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_activate()
	{
		if( Sprite::_activate() == false )
		{
			return false;
		}

		if( m_resourceSequenceName.empty() == true )
		{
			return false;
		}

		m_currentFrame = 0;

		if( m_resourceSequence == NULL )
		{
			MENGE_LOG_ERROR( "Animation: '%s' Image resource not getting '%s'"
				, getName().c_str()
				, m_resourceSequenceName.c_str() 
				);

			return false;
		}

		std::size_t currentImageIndex = m_resourceSequence->getSequenceIndex( m_currentFrame );
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

		if( m_resourceSequenceName.empty() == false )
		{	
			m_resourceSequence = ResourceManager::get()
				->getResourceT<ResourceSequence>( m_resourceSequenceName );

			if( m_resourceSequence == 0 )
			{
				MENGE_LOG_ERROR( "Animation: '%s' no found resource with name '%s'"
					, m_name.c_str()
					, m_resourceSequenceName.c_str() 
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
			->releaseResource( m_resourceSequence );
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
		m_timinig = 0.f;

		this->updateCurrentImageIndex_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_restart( std::size_t _enumerator )
	{
		//Todo

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_stop( std::size_t _enumerator )
	{
		//m_currentFrame = 0;
		//m_timinig = 0.f;

		this->updateCurrentImageIndex_();

		if( m_onEndAnimationEvent == true )
		{
			this->callEvent( EVENT_ANIMATION_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_end( std::size_t _enumerator )
	{
		//m_currentFrame = 0;
		//m_timinig = 0.f;
		m_currentFrame=m_resourceSequence->getLastFrameIndex();
		
		this->updateCurrentImageIndex_();

		if( m_onEndAnimationEvent == true )
		{
			this->callEvent( EVENT_ANIMATION_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::updateCurrentImageIndex_()
	{
		std::size_t currentImageIndex = m_resourceSequence->getSequenceIndex( m_currentFrame );
		this->setImageIndex( currentImageIndex );
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
				, m_resourceSequenceName.c_str()
				);

			return 0;
		}

		return m_resourceSequence->getSequenceCount();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getFrameDelay( std::size_t  _frame ) const
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' getFrameDelay not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceSequenceName.c_str()
				);

			return 0;
		}

		return m_resourceSequence->getSequenceDelay( _frame );
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

		std::size_t sequenceCount = m_resourceSequence->getSequenceCount();

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

		this->updateCurrentImageIndex_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setTimming( float _timming )
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' setTimming not activate"
				, m_name.c_str()
				);

			return;
		}

		m_timinig = _timming;

		this->updateCurrentImageIndex_();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getTimming() const
	{
		return m_timinig;
	}
}
