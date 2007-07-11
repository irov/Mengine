#	include "ResourceImageCell.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageCell )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageCell::ResourceImageCell( const std::string & _name )
		: ResourceImageDefault( _name )
		, m_numX(0)
		, m_numY(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageCell::getCount()
	{
		return m_uvs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageCell::getUV( size_t _index )
	{
		return m_uvs[ _index ];
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCell::loader( TiXmlElement * _xml )
	{
		ResourceImageDefault::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "Cell", "X", m_numX );
			XML_CHECK_VALUE_NODE( "Cell", "Y", m_numY );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCell::_compile()
	{
		ResourceImageDefault::_compile();

		m_count = m_numX * m_numY;

		m_uvs.resize( m_count );
		
		m_size.x = m_size.x / m_numX;
		m_size.y = m_size.y / m_numY;

		for( size_t index = 0; index < m_count; ++index )
		{
			size_t offset = index / m_numY;

			m_uvs[index].x = float( index % m_numX ) / m_numX;
			m_uvs[index].y = float( offset ) / m_numY;
			m_uvs[index].z = float( index % m_numX + 1 ) / m_numX;
			m_uvs[index].w = float( offset + 1 ) / m_numY;
		}

		return true;
	}
}