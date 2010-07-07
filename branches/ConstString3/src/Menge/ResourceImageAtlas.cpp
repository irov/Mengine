#	include "ResourceImageAtlas.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

//#	include <FreeImage.h>

#	include <sstream>

#	include "Utils/Core/PixelFormat.h"

//#	include "ImageCodec.h"

#	include "RenderEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageAtlas )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageAtlas::ResourceImageAtlas()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageAtlas::getMaxSize( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].maxSize;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageAtlas::getCount() const
	{
		return m_vectorImageFrames.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageAtlas::getFilename( std::size_t _frame ) const
	{
		return m_vectorImageDescs[ _frame ].fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageAtlas::getCodecType( std::size_t _frame ) const
	{
		return m_vectorImageDescs[ _frame ].codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageAtlas::getFilenameCount() const
	{
		//õç
		return m_vectorImageDescs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageAtlas::getSize( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageAtlas::getOffset( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageAtlas::getUV( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].uv;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceImageAtlas::getTexture( std::size_t _frame )
	{
		return m_vectorImageFrames[ _frame ].texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageAtlas::isAlpha( std::size_t _frame ) const
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

				if( desc.codecType.empty() )
				{
					desc.codecType = s_getImageCodec( desc.fileName.str() );
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
			
			if( loadImageFrame_( frame, category, it->fileName, it->codecType ) == false )
			{
				return false;
			}

			frame.uv = it->uv;
			
		//	frame.maxSize = frame.size;

			frame.maxSize = it->maxSize;
			frame.offset =  it->offset;

			frame.size = it->size;

			/*if( frame.maxSize.x < 0.f || frame.maxSize.y < 0.f )
			{
				frame.maxSize = frame.size;
			}
			else
			{
				frame.size = it->size;
			}*/

			//frame.size = it->size;
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
			releaseImageFrame_( *it );
		}

		m_vectorImageFrames.clear();
	}
}
