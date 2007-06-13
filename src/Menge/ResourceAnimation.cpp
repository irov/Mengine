#	include "ResourceAnimation.h"
#	include "ResourceImplement.h"

#	include "XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceAnimation )
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimation::ResourceAnimation( const std::string & _name )
	: ResourceImpl( _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getSequenceCount() const
	{
		return m_vectorSequence.size();
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceAnimation::getSequenceDelay( size_t _sequence ) const
	{
		return m_vectorSequence[ _sequence ].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getSequenceIndex( size_t _sequence ) const
	{
		return m_vectorSequence[ _sequence ].index;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & ResourceAnimation::getImageMap() const
	{
		return m_imageMap;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::loader( TiXmlElement * _xml )
	{
		ResourceImpl::loader( _xml );

		//<ImageMap Name = "output_cng" />
		//	<Sequences>
		//	<Sequence Frame = "1" Delay = "50" />
		//	<Sequence Frame = "2" Delay = "50" />
		//	<Sequence Frame = "3" Delay = "50" />
		//	<Sequence Frame = "4" Delay = "50" />
		//	<Sequence Frame = "5" Delay = "50" />
		//	</Sequences>

		Sequence sq;

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "ImageMap", "Name", m_imageMap );

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
