#	include "ResourceImageSet.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"

#	include "Utils/Core/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageSet );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSet::ResourceImageSet()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSet::getMaxSize( size_t _frame ) const
	{
		return m_sizes[_frame];
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSet::getSize( size_t _frame ) const
	{
		return m_sizes[_frame];
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageSet::getCount() const 
	{
		return m_uvs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSet::getOffset( size_t _frame ) const
	{
		return m_imageFrame.offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageSet::getUV( size_t _frame ) const
	{
		return m_uvs[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageSet::getUVImage( size_t _frame ) const
	{
		return m_uvs[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSet::isAlpha( size_t _frame ) const
	{
		assert(!"ResourceImageSet::isAlpha not implemented");
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceImageSet::getTexture( size_t _frame ) const
	{
		return m_imageFrame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageSet::getFileName( size_t _frame ) const
	{
		return m_imageDesc.fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageSet::getCodecType( size_t _frame ) const
	{
		return m_imageDesc.codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageSet::getFilenameCount() const
	{
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSet::addFrameUV( const mt::vec4f & _uv )
	{
		m_uvs.push_back( _uv );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSet::loader( BinParser * _parser )
	{
		ResourceImage::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::File )
			{
				m_imageDesc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
				m_imageDesc.offset = mt::vec2f(0.f,0.f);
				m_imageDesc.maxSize = mt::vec2f(0.f,0.f);

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::File_Path, m_imageDesc.fileName );
					BIN_CASE_ATTRIBUTE( Protocol::File_Codec, m_imageDesc.codecType );
					BIN_CASE_ATTRIBUTE( Protocol::File_UV, m_imageDesc.uv );
					BIN_CASE_ATTRIBUTE( Protocol::File_Offset, m_imageDesc.offset );
					BIN_CASE_ATTRIBUTE( Protocol::File_MaxSize, m_imageDesc.maxSize );
				}
			}

			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Frame_UV, &ResourceImageSet::addFrameUV );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSet::_compile()
	{
		const ConstString & category = this->getCategory();

		if( loadImageFrame_( m_imageFrame, category, m_imageDesc.fileName, m_imageDesc.codecType ) == false )
		{
			return false;
		}

		m_imageFrame.uv = m_imageDesc.uv;
		m_imageFrame.maxSize = m_imageDesc.maxSize;
		m_imageFrame.offset = m_imageDesc.offset;

		float ku = m_imageFrame.uv.z - m_imageFrame.uv.x;
		float kv = m_imageFrame.uv.w - m_imageFrame.uv.y;

		m_imageFrame.size = mt::vec2f(m_imageFrame.size.x * ku, m_imageFrame.size.y * kv);

		TVectorSizes::size_type uv_size = m_uvs.size();
		
		if( uv_size == 0 )
		{
			return false;
		}

		m_sizes.resize( uv_size );

		for( TVectorSizes::size_type frame = 0; frame != uv_size; ++frame )
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
