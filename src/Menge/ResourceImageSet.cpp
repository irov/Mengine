#	include "ResourceImageSet.h"
#	include "ResourceImplement.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageSet )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSet::ResourceImageSet( const std::string & _name )
		: ResourceImageCell( _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSet::getMaxSize( size_t _frame ) const
	{
		return m_sizes[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSet::getSize( size_t _frame ) const
	{
		return m_sizes[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSet::loader( TiXmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE( "Frame" )
			{
				mt::vec4f val;
				XML_VALUE_ATTRIBUTE("UV", val);
				m_uvs.push_back( val );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSet::_compile()
	{
		std::vector<char> buff;
		ImageFrame image = loadImageFrame( m_fileImage, buff );

		size_t size = m_uvs.size();
		m_sizes.resize( size );

		for( size_t frame = 0; frame != size; ++frame )
		{
			float u = m_uvs[ frame ].z - m_uvs[ frame ].x;
			float v = m_uvs[ frame ].w - m_uvs[ frame ].y;
			m_sizes[ frame ].x = m_imageFrame.size.x * u;
			m_sizes[ frame ].y = m_imageFrame.size.y * v;
		}

		return true;
	}
}