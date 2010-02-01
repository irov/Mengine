#	include "ResourceAnimation.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceAnimation )
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
	void ResourceAnimation::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Sequences" )
			{
				XML_PARSE_ELEMENT( this, &ResourceAnimation::loaderSequences_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::loaderSequences_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Sequence" )
			{
				Sequence sq;
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Index", sq.index );
					XML_CASE_ATTRIBUTE( "Delay", sq.delay );
					//if(abs(sq.delay) > 10000) continue;
				}
				m_vectorSequence.push_back( sq );
			}
			XML_CASE_NODE( "SequenceArray" )
			{
				std::size_t count = 0;
				float delay;
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Count", count );
					XML_CASE_ATTRIBUTE( "Delay", delay );
				}
				for( std::size_t i = 0; i < count; i++ )
				{
					Sequence sq;
					sq.delay = delay;
					sq.index = m_vectorSequence.size();
					m_vectorSequence.push_back( sq );
				}
			}
		}
	}


}
