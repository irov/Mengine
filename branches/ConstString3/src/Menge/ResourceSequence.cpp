#	include "ResourceSequence.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceSequence );
	//////////////////////////////////////////////////////////////////////////
	ResourceSequence::ResourceSequence()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceSequence::getSequenceCount() const
	{
		return m_sequence.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceSequence::getSequenceDelay( std::size_t _sequence ) const
	{
		return m_sequence[_sequence].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceSequence::getSequenceIndex( std::size_t _sequence ) const
	{
		return m_sequence[_sequence].index;
	}	
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceSequence::getLastFrameIndex() const
	{
		return m_sequence.size() - 1;	
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSequence::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE_PARSE_METHOD( Protocol::Sequences, this, &ResourceSequence::loaderSequences_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSequence::loaderSequences_( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Sequence )
			{
				AnimationSequence sq;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Sequence_Index, sq.index );
					BIN_CASE_ATTRIBUTE( Protocol::Sequence_Delay, sq.delay );
					//if(abs(sq.delay) > 10000) continue;
				}
				
				m_sequence.push_back( sq );
			}
			BIN_CASE_NODE( Protocol::SequenceArray )
			{
				std::size_t count = 0;
				float delay;
				BIN_FOR_EACH_ATTRIBUTES()
				{					
					BIN_CASE_ATTRIBUTE( Protocol::SequenceArray_Count, count );
					BIN_CASE_ATTRIBUTE( Protocol::SequenceArray_Delay, delay );
				}

				for( std::size_t i = 0; i != count; ++i )
				{
					AnimationSequence sq;
					sq.delay = delay;
					sq.index = m_sequence.size();
					m_sequence.push_back( sq );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSequence::setSequences( const TVectorAnimationSequence & _sequence )
	{
		m_sequence = _sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorAnimationSequence & ResourceSequence::getSequences() const
	{
		return m_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSequence::_compile()
	{
		if( m_sequence.empty() )
		{
			MENGE_LOG_ERROR( "Animation: sequence count is empty '%s'"
				, getName().c_str()
				);

			return false;
		}

		return true;
	}
}
