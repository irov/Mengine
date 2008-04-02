#	include "ResourceImageSet.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageSet );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSet::ResourceImageSet( const ResourceFactoryParam & _params )
	: ResourceImageCell( _params )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSet::getMaxSize( unsigned int _frame ) const
	{
		return m_sizes[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSet::getSize( unsigned int _frame ) const
	{
		return m_sizes[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSet::addFrameUV( const mt::vec4f & _uv )
	{
		m_uvs.push_back( _uv );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSet::loader( XmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceImageCell::setFilePath );

			XML_CASE_NODE( "Frame" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "UV", &ResourceImageSet::addFrameUV );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSet::_compile()
	{
		m_imageFrame = loadImageFrame( m_filename );

		TVectorSizes::size_type size = m_uvs.size();
		
		if( size == 0 )
		{
			return false;
		}

		m_sizes.resize( size );

		for( TVectorSizes::size_type frame = 0; frame != size; ++frame )
		{
			TVectorUV::value_type & uv = m_uvs[ frame ];

			float u = uv.z - uv.x;
			float v = uv.w - uv.y;

			TVectorSizes::value_type & size = m_sizes[ frame ];

			size.x = m_imageFrame.size.x * u;
			size.y = m_imageFrame.size.y * v;
		}

		return true;
	}
}