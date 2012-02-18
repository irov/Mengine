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

		this->recompile();
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

		size_t frameSize = m_resourceSequence->getSequenceCount();

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
					return;
				}
			}	

			delay = m_resourceSequence->getSequenceDelay( m_currentFrame );
			delay *= m_animationFactor;
		}

		size_t currentImageIndex = m_resourceSequence->getSequenceIndex( m_currentFrame );
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

		size_t currentImageIndex = m_resourceSequence->getSequenceIndex( m_currentFrame );
		this->setImageIndex( currentImageIndex );

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
				
		if( m_resourceSequence != 0 )
		{
			m_resourceSequence->decrementReference();
			m_resourceSequence = 0;
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
		m_timinig = 0.f;

		this->updateCurrentImageIndex_();

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
		m_timinig = 0.f;

		this->updateCurrentImageIndex_();

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

		m_timinig = 0.f;
		m_currentFrame = 0;
		
		this->updateCurrentImageIndex_();

		if( m_onEndAnimationEvent == true )
		{
			this->callEvent( EVENT_ANIMATION_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::updateCurrentImageIndex_()
	{
		size_t currentImageIndex = m_resourceSequence->getSequenceIndex( m_currentFrame );
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
	size_t Animation::getCurrentFrame() const
	{
		return m_currentFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Animation::getFrameCount() const
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation.getFrameCount: '%s' not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceSequenceName.c_str()
				);

			return 0;
		}

		return m_resourceSequence->getSequenceCount();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getFrameDelay( size_t  _frame ) const
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation.getFrameDelay: '%s' not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceSequenceName.c_str()
				);

			return 0;
		}

		return m_resourceSequence->getSequenceDelay( _frame );
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

		size_t sequenceCount = m_resourceSequence->getSequenceCount();

		if( _frame >= sequenceCount || _frame < 0 )	
		{
			MENGE_LOG_ERROR( "Animation.setCurrentFrame: '%s' _frame(%d) <> sequenceCount(%d)"
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
	void Animation::setTiming( float _timing )
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' setTiming not activate"
				, m_name.c_str()
				);

			return;
		}

		m_timinig = _timing;

		this->updateCurrentImageIndex_();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getTiming() const
	{
		return m_timinig;
	}
}
