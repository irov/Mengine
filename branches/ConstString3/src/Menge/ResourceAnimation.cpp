#	include "ResourceAnimation.h"
#	include "ResourceImplement.h"

#	include "ResourceImage.h"
#	include "ResourceAnimation.h"

#	include "ResourceManager.h"

#	include "BinParser.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceAnimation );
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimation::ResourceAnimation()
		: m_duration(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Sequence )
			{
				AnimationSequence sq;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Sequence_ResourceImageName, sq.resourceName );
					BIN_CASE_ATTRIBUTE( Protocol::Sequence_Delay, sq.delay );
				}

				m_sequence.push_back( sq );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceAnimation::_compile()
	{
		if( m_sequence.empty() )
		{
			MENGE_LOG_ERROR( "Animation: sequence count is empty '%s'"
				, getName().c_str()
				);

			return false;
		}

		m_duration = 0.f;

		for( TVectorAnimationSequence::iterator
			it = m_sequence.begin(),
			it_end = m_sequence.end();
		it != it_end;
		++it )
		{
			AnimationSequence & sequence = *(it);
			
			ResourceImage * resource  = ResourceManager::get()
				->getResourceT<ResourceImage>( sequence.resourceName );

			if( resource == 0 )
			{
				MENGE_LOG_ERROR( "ResourceAnimation: '%s' Image resource not found resource '%s'"
					, m_name.c_str()
					, sequence.resourceName.c_str() 
					);

				return false;
			}

			sequence.resource = resource;

			m_duration += sequence.delay;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::_release()
	{
		for( TVectorAnimationSequence::iterator
			it = m_sequence.begin(),
			it_end = m_sequence.end();
		it != it_end;
		++it )
		{
			AnimationSequence & sequence = *(it);

			if( sequence.resource == NULL )
			{
				continue;
			}
			
			sequence.resource->decrementReference();
			sequence.resource = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getSequenceCount() const
	{
		return m_sequence.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceAnimation::getSequenceDelay( size_t _sequence ) const
	{
		return m_sequence[_sequence].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceAnimation::getSequenceResourceName( size_t _sequence ) const
	{
		return m_sequence[_sequence].resourceName;
	}	
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getLastFrameIndex() const
	{
		return m_sequence.size() - 1;	
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::setSequences( const TVectorAnimationSequence & _sequence )
	{
		m_sequence = _sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorAnimationSequence & ResourceAnimation::getSequences() const
	{
		return m_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage * ResourceAnimation::getSequenceResource( size_t _sequence ) const
	{
		return m_sequence[_sequence].resource;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceAnimation::getSequenceDuration() const
	{
		return m_duration;
	}
}
