#	include "ResourceImageCell.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageCell );
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
	const ConstString & ResourceImageCell::getFileName( size_t _frame ) const
	{
		return m_imageDesc.fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageCell::getCodecType( size_t _frame ) const
	{
		return m_imageDesc.codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageCell::getFilenameCount() const
	{
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getSize( size_t _frame ) const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCell::getOffset( size_t _frame ) const
	{
		return m_imageFrame.offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageCell::getUV( size_t _frame ) const
	{
		return m_uvs[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageCell::getUVImage( size_t _frame ) const
	{
		return m_uvs[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCell::isAlpha( size_t _frame ) const
	{
		return m_imageFrame.isAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	TextureInterface* ResourceImageCell::getTexture( size_t _frame ) const
	{
		return m_imageFrame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageCell::getNumX() const 
	{
		return m_numX;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageCell::getNumY() const
	{
		return m_numY;
	}	
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCell::loader( BinParser * _parser )
	{
		ResourceImage::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::File )
			{
				ImageDesc desc;
				desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
				desc.offset = mt::vec2f(0.f,0.f);
				desc.maxSize = mt::vec2f(0.f,0.f);
				desc.size = mt::vec2f(0.f,0.f);
				desc.isAlpha = true;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::File_Path, desc.fileName );
					BIN_CASE_ATTRIBUTE( Protocol::File_Codec, desc.codecType );
					BIN_CASE_ATTRIBUTE( Protocol::File_UV, desc.uv );
					BIN_CASE_ATTRIBUTE( Protocol::File_Offset, desc.offset );
					BIN_CASE_ATTRIBUTE( Protocol::File_MaxSize, desc.maxSize );
					BIN_CASE_ATTRIBUTE( Protocol::File_Size, desc.size );
					BIN_CASE_ATTRIBUTE( Protocol::File_Alpha, desc.isAlpha );
				}

				if( desc.codecType.empty() )
				{
					desc.codecType = s_getImageCodec( desc.fileName );
				}

				m_imageDesc = desc;
			}

			BIN_CASE_ATTRIBUTE( Protocol::Cell_X, m_numX );
			BIN_CASE_ATTRIBUTE( Protocol::Cell_Y, m_numY );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCell::_compile()
	{
		const ConstString & category = this->getCategory();

		if( loadImageFrame_( m_imageFrame, category, m_imageDesc.fileName, m_imageDesc.codecType ) == false )
		{
			return false;
		}

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
