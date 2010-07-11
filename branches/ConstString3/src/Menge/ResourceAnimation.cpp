#	include "ResourceAnimation.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"
#	include "Logger/Logger.h"

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
		return m_vectorSequence.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceAnimation::getSequenceDelay( std::size_t _sequence ) const
	{
		return m_vectorSequence[ _sequence ].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceAnimation::getSequenceIndex( std::size_t _sequence ) const
	{
		return m_vectorSequence[ _sequence ].index;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE_PARSE_ELEMENT( Protocol::Sequences, this, &ResourceAnimation::loaderSequences_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::loaderSequences_( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Sequence )
			{
				Sequence sq;
				BIN_FOR_EACH_ATTRIBUTES()
				{					
					BIN_CASE_ATTRIBUTE( Protocol::Sequence_Index, sq.index );
					BIN_CASE_ATTRIBUTE( Protocol::Sequence_Delay, sq.delay );
					//if(abs(sq.delay) > 10000) continue;
				}
				
				m_vectorSequence.push_back( sq );
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
					Sequence sq;
					sq.delay = delay;
					sq.index = m_vectorSequence.size();
					m_vectorSequence.push_back( sq );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceAnimation::_compile()
	{
		if( m_vectorSequence.empty() )
		{
			MENGE_LOG_ERROR( "Animation: sequence count is empty '%s'"
				, getName().c_str()
				);

			return false;
		}

		return true;
	}
}
