#	include "ResourceAnimation.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceAnimation )
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimation::ResourceAnimation( const std::string & _name )
	: ResourceReference( _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getSequenceCount() const
	{
		return m_vectorSequence.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceAnimation::getSequenceDelay( size_t _sequence ) const
	{
		return m_vectorSequence[ _sequence ].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getSequenceIndex( size_t _sequence ) const
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
				XML_FOR_EACH_ATTRIBUTES()
				{
					Sequence sq;
					XML_CASE_ATTRIBUTE( "Index", sq.index );
					XML_CASE_ATTRIBUTE( "Delay", sq.delay );
					m_vectorSequence.push_back( sq );
				}
			}
		}
	}


}