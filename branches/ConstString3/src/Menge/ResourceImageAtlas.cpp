#	include "ResourceImageAtlas.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"

//#	include <FreeImage.h>

#	include <sstream>

#	include "Utils/Core/PixelFormat.h"

//#	include "ImageCodec.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageAtlas );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageAtlas::ResourceImageAtlas()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageAtlas::getMaxSize( size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].maxSize;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageAtlas::getCount() const
	{
		return m_vectorImageFrames.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageAtlas::getFileName( size_t _frame ) const
	{
		return m_vectorImageDescs[ _frame ].fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageAtlas::getCodecType( size_t _frame ) const
	{
		return m_vectorImageDescs[ _frame ].codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageAtlas::getFilenameCount() const
	{
		//õç
		return m_vectorImageDescs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageAtlas::getSize( size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageAtlas::getOffset( size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageAtlas::getUV( size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageAtlas::getUVImage( size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].uv_image;
	}
	//////////////////////////////////////////////////////////////////////////
	TextureInterface* ResourceImageAtlas::getTexture( size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageAtlas::isAlpha( size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].isAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageAtlas::loader( BinParser * _parser )
	{
		ResourceImage::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::File )
			{
				ImageDesc desc;
				desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
				desc.offset = mt::vec2f(0.f,0.f);
				desc.maxSize = mt::vec2f(-1.f,-1.f);
				desc.size = mt::vec2f(-1.f,-1.f);
				desc.isAlpha = true; //

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

				if( desc.codecType.empty() == true )
				{
					desc.codecType = s_getImageCodec( desc.fileName );
				}

				m_vectorImageDescs.push_back( desc );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageAtlas::_compile()
	{	
		for( TVectorImageDesc::iterator
			it = m_vectorImageDescs.begin(),
			it_end = m_vectorImageDescs.end();
		it != it_end;
		++it)
		{
			const ConstString & category = this->getCategory();

			ImageFrame frame;
			
			if( this->loadImageFrame_( frame, category, it->fileName, it->codecType ) == false )
			{
				return false;
			}

			frame.uv = it->uv;
			frame.uv_image = it->uv;

			frame.maxSize = it->maxSize;
			frame.offset =  it->offset;
			frame.size = it->size;
			frame.isAlpha = it->isAlpha;

			m_vectorImageFrames.push_back( frame );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageAtlas::_release()
	{
		for( TVectorImageFrame::iterator
			it = m_vectorImageFrames.begin(),
			it_end = m_vectorImageFrames.end();
		it != it_end;
		++it)
		{
			this->releaseImageFrame_( *it );
		}

		m_vectorImageFrames.clear();
	}
}
