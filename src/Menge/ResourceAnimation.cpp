#	include "ResourceAnimation.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceAnimation )
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimation::ResourceAnimation( const ResourceFactoryParam & _params )
	: ResourceReference( _params )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceAnimation::getSequenceCount() const
	{
		return m_vectorSequence.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceAnimation::getSequenceDelay( unsigned int _sequence ) const
	{
		return m_vectorSequence[ _sequence ].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ResourceAnimation::getSequenceIndex( unsigned int _sequence ) const
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
		}
	}


}