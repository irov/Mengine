#	include "ResourceAnimation.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceAnimation );
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimation::ResourceAnimation()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceAnimation::getSequenceCount() const
	{
		return m_sequence.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceAnimation::getSequenceDelay( std::size_t _sequence ) const
	{
		return m_sequence[ _sequence ].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceAnimation::getSequenceIndex( std::size_t _sequence ) const
	{
		return m_sequence[ _sequence ].index;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE_PARSE_METHOD( Protocol::Sequences, this, &ResourceAnimation::loaderSequences_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::loaderSequences_( BinParser * _parser )
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
	bool ResourceAnimation::_compile()
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
