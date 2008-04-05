#	include "ResourceImageCell.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageCell )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageCell::ResourceImageCell( const ResourceFactoryParam & _params )
		: ResourceImage( _params )
		, m_numX(0)
		, m_numY(0)
		//, m_offset(0.f,0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getMaxSize( unsigned int _frame ) const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ResourceImageCell::getCount() const 
	{
		return m_uvs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getSize( unsigned int _frame ) const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getOffset( unsigned int _frame ) const
	{
		return m_vectorAtlasOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageCell::getUV( unsigned int _frame ) const
	{
		return m_uvs[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageCell::getImage( unsigned int _frame ) const
	{
		return m_imageFrame.image;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCell::setFilePath( const std::string & _path )
	{
		m_filename = m_params.category + _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCell::loader( XmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceImageCell::setFilePath );
			XML_CASE_ATTRIBUTE( "UV", m_vectorAtlasUV );
			XML_CASE_ATTRIBUTE( "Offset", m_vectorAtlasOffset );
			XML_CASE_ATTRIBUTE( "MaxSize", m_vectorAtlasMaxSize );

			XML_CASE_NODE( "Cell" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "X", m_numX );
					XML_CASE_ATTRIBUTE( "Y", m_numY );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCell::_compile()
	{
		m_imageFrame = loadImageFrame( m_filename );

		TVectorUV::size_type count = m_numX * m_numY;

		float u = m_imageFrame.uv.z - m_imageFrame.uv.x;
		float v = m_imageFrame.uv.w - m_imageFrame.uv.y;

		mt::vec2f size(m_imageFrame.size.x * u,m_imageFrame.size.y * v);
		m_imageFrame.size = size;

		m_imageFrame.size.x /= (float)m_numX;
		m_imageFrame.size.y /= (float)m_numY;

		m_uvs.resize( count );

		for( TVectorUV::size_type index = 0; index < count; ++index )
		{
			TVectorUV::size_type offset = index / m_numX;

			m_uvs[index].x = float( index % m_numX ) / m_numX;
			m_uvs[index].y = float( offset ) / m_numY;
			m_uvs[index].z = float( index % m_numX + 1 ) / m_numX;
			m_uvs[index].w = float( offset + 1 ) / m_numY;
			m_uvs[index] += m_vectorAtlasUV;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCell::_release()
	{
		releaseImageFrame( m_imageFrame );
	}
}