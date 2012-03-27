//#	include "ResourceSequence.h"
//
//#	include "ResourceImplement.h"
//
//#	include "BinParser.h"
//#	include "LogEngine.h"
//
//namespace Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	RESOURCE_IMPLEMENT( ResourceSequence );
//	//////////////////////////////////////////////////////////////////////////
//	ResourceSequence::ResourceSequence()
//	{
//	}
//	//////////////////////////////////////////////////////////////////////////
//	size_t ResourceSequence::getSequenceCount() const
//	{
//		return m_sequence.size();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float ResourceSequence::getSequenceDelay( size_t _sequence ) const
//	{
//		return m_sequence[_sequence].delay;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	size_t ResourceSequence::getSequenceIndex( size_t _sequence ) const
//	{
//		return m_sequence[_sequence].index;
//	}	
//	//////////////////////////////////////////////////////////////////////////
//	size_t ResourceSequence::getLastFrameIndex() const
//	{
//		return m_sequence.size() - 1;	
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceSequence::loader( BinParser * _parser )
//	{
//		ResourceReference::loader( _parser );
//
//		BIN_SWITCH_ID( _parser )
//		{
//			BIN_CASE_NODE_PARSE_METHOD( Protocol::Sequences, this, &ResourceSequence::loaderSequences_ );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceSequence::loaderSequences_( BinParser * _parser )
//	{
//		BIN_SWITCH_ID( _parser )
//		{
//			BIN_CASE_NODE( Protocol::Sequence )
//			{
//				AnimationSequence sq;
//				BIN_FOR_EACH_ATTRIBUTES()
//				{
//					BIN_CASE_ATTRIBUTE( Protocol::Sequence_Index, sq.index );
//					BIN_CASE_ATTRIBUTE( Protocol::Sequence_Delay, sq.delay );
//					//if(abs(sq.delay) > 10000) continue;
//				}
//				
//				this->addSequence( sq );
//			}
//			BIN_CASE_NODE( Protocol::SequenceArray )
//			{
//				size_t count = 0;
//				float delay = 0.f;
//
//				BIN_FOR_EACH_ATTRIBUTES()
//				{					
//					BIN_CASE_ATTRIBUTE( Protocol::SequenceArray_Count, count );
//					BIN_CASE_ATTRIBUTE( Protocol::SequenceArray_Delay, delay );
//				}
//
//				for( size_t i = 0; i != count; ++i )
//				{
//					AnimationSequence sq;
//					sq.delay = delay;
//					sq.index = m_sequence.size();
//
//					this->addSequence( sq );
//				}
//			}
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceSequence::addSequence( const AnimationSequence & _sequence )
//	{
//		if( _sequence.delay < 0.00001f )
//		{
//			MENGE_LOG_ERROR("Animation::update %s sequence %d delay is ~zero"
//				, m_name.c_str()
//				, _sequence.index
//				);
//
//			return;
//		}
//
//		m_sequence.push_back( _sequence );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceSequence::setSequences( const TVectorAnimationSequence & _sequence )
//	{
//		m_sequence = _sequence;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	const TVectorAnimationSequence & ResourceSequence::getSequences() const
//	{
//		return m_sequence;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool ResourceSequence::_compile()
//	{
//		if( m_sequence.empty() )
//		{
//			MENGE_LOG_ERROR( "Animation: sequence count is empty '%s'"
//				, getName().c_str()
//				);
//
//			return false;
//		}
//
//		return true;
//	}
//}
