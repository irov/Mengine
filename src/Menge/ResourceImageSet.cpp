#	include "ResourceImageSet.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

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
			XML_CASE_ATTRIBUTE_NODE( "File", "Path", m_filename );

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

		size_t size = m_uvs.size();
		
		if( size == 0 )
		{
			return false;
		}

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