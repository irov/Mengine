#	include "ResourceImageCell.h"

#	include "ResourceImplement.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageCell )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageCell::ResourceImageCell( const std::string & _name )
		: ResourceImage( _name )
		, m_numX(0)
		, m_numY(0)
		, m_offset(0.f,0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getMaxSize( size_t _frame ) const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageCell::getCount() const 
	{
		return m_uvs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getSize( size_t _frame ) const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getOffset( size_t _frame ) const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageCell::getUV( size_t _frame ) const
	{
		return m_uvs[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageCell::getImage( size_t _frame ) const
	{
		return m_imageFrame.image;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCell::loader( TiXmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_filename );
			XML_CHECK_VALUE_NODE( "Cell", "X", m_numX );
			XML_CHECK_VALUE_NODE( "Cell", "Y", m_numY );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCell::_compile()
	{
		std::vector<char> buff;
		m_imageFrame = loadImageFrame( m_filename, buff );

		size_t count = m_numX * m_numY;

		m_imageFrame.size.x /= (float)m_numX;
		m_imageFrame.size.y /= (float)m_numY;

		m_uvs.resize( count );

		for( size_t index = 0; index < count; ++index )
		{
			size_t offset = index / m_numX;

			m_uvs[index].x = float( index % m_numX ) / m_numX;
			m_uvs[index].y = float( offset ) / m_numY;
			m_uvs[index].z = float( index % m_numX + 1 ) / m_numX;
			m_uvs[index].w = float( offset + 1 ) / m_numY;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCell::_release()
	{
		releaseImageFrame( m_imageFrame );
	}
}