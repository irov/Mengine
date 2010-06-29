#	include "ResourceImageCell.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageCell )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageCell::ResourceImageCell()
		: m_numX(0)
		, m_numY(0)
	{
		m_imageDesc.isAlpha = true;
		m_imageDesc.maxSize = mt::vec2f( 0.0f, 0.0f );
		m_imageDesc.offset = mt::vec2f( 0.0f, 0.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getMaxSize( std::size_t _frame ) const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageCell::getCount() const 
	{
		return m_uvs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageCell::getFilename( std::size_t _frame ) const
	{
		return m_imageDesc.fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageCell::getCodecType( std::size_t _frame ) const
	{
		return m_imageDesc.codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageCell::getFilenameCount() const
	{
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getSize( std::size_t _frame ) const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getOffset( std::size_t _frame ) const
	{
		return m_imageFrame.offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageCell::getUV( std::size_t _frame ) const
	{
		return m_uvs[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCell::isAlpha( std::size_t _frame ) const
	{
		return m_imageFrame.isAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceImageCell::getTexture( std::size_t _frame )
	{
		return m_imageFrame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageCell::getNumX() const 
	{
		return m_numX;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageCell::getNumY() const
	{
		return m_numY;
	}	
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCell::loader( XmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "File" )
			{
				ImageDesc desc;
				desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
				desc.offset = mt::vec2f(0.f,0.f);
				desc.maxSize = mt::vec2f(0.f,0.f);
				desc.size = mt::vec2f(0.f,0.f);
				desc.isAlpha = true;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", desc.fileName );
					XML_CASE_ATTRIBUTE( "Codec", desc.codecType );
					XML_CASE_ATTRIBUTE( "UV", desc.uv );
					XML_CASE_ATTRIBUTE( "Offset", desc.offset );
					XML_CASE_ATTRIBUTE( "MaxSize", desc.maxSize );
					XML_CASE_ATTRIBUTE( "Size", desc.size );
					XML_CASE_ATTRIBUTE( "Alpha", desc.isAlpha );
				}

				if( desc.codecType.invalid() )
				{
					desc.codecType = s_getImageCodec( desc.fileName.str() );
				}

				m_imageDesc = desc;
			}
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
		const ConstString & category = this->getCategory();
		m_imageFrame = loadImageFrame_( category, m_imageDesc.fileName, m_imageDesc.codecType );

		m_imageFrame.uv = m_imageDesc.uv;
		m_imageFrame.maxSize = m_imageDesc.maxSize;
		m_imageFrame.offset = m_imageDesc.offset;

	/*	float ku = m_imageFrame.uv.z - m_imageFrame.uv.x;
		float kv = m_imageFrame.uv.w - m_imageFrame.uv.y;

		m_imageFrame.size = mt::vec2f(m_imageFrame.size.x * ku,m_imageFrame.size.y * kv);
	*/
		if( m_imageFrame.size.x == 0.0f &&
			m_imageFrame.size.y == 0.0f )
		{
			m_imageFrame.size = m_imageDesc.size;
		}

		if( m_imageFrame.maxSize.x == 0.0f &&
			m_imageFrame.maxSize.y == 0.0f )
		{
			m_imageFrame.maxSize = m_imageFrame.size;
		}

		m_imageFrame.size.x /= (float)m_numX;
		m_imageFrame.size.y /= (float)m_numY;

		m_imageFrame.maxSize.x /= (float)m_numX;
		m_imageFrame.maxSize.y /= (float)m_numY;

		m_imageFrame.isAlpha = m_imageDesc.isAlpha;

		TVectorUV::size_type count = m_numX * m_numY;

		m_uvs.resize( count );

		for( TVectorUV::size_type index = 0; index < count; ++index )
		{
			TVectorUV::size_type offset = index / m_numX;

			m_uvs[index].x = float( index % m_numX ) / m_numX;
			m_uvs[index].y = float( offset ) / m_numY;
			m_uvs[index].z = float( index % m_numX + 1 ) / m_numX;
			m_uvs[index].w = float( offset + 1 ) / m_numY;

			//m_uvs[index] += m_imageFrame.offset / m_imageFrame.size; // ?? или при getUV ??
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCell::_release()
	{
		releaseImageFrame_( m_imageFrame );
	}
}
