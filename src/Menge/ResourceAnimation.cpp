#	include "ResourceAnimation.h"
#	include "ResourceImplement.h"

#	include "XmlParser/XmlParser.h"

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
	size_t ResourceAnimation::getSequenceDelay( size_t _sequence ) const
	{
		return m_vectorSequence[ _sequence ].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getSequenceIndex( size_t _sequence ) const
	{
		return m_vectorSequence[ _sequence ].index;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::loader( TiXmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		Sequence sq;

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE_FOR_EACH( "Sequences" )
			{
				XML_CHECK_NODE( "Sequence" )
				{
					XML_VALUE_ATTRIBUTE( "Index", sq.index );
					XML_VALUE_ATTRIBUTE( "Delay", sq.delay );

					m_vectorSequence.push_back( sq );
				}
			}		
		}
	}
}