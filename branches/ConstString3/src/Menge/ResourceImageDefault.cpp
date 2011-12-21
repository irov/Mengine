#	include "ResourceImageDefault.h"

#	include "ResourceImplement.h"

#	include <cstdio>

#	include "BinParser.h"
#	include "Core/ConstString.h"
#	include "Utils/Core/String.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageDefault );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::ResourceImageDefault()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getMaxSize( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].maxSize;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageDefault::getCount() const
	{
		return m_vectorImageFrames.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getSize( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getOffset( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDefault::getUV( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDefault::getUVImage( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].uv_image;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceImageDefault::getTexture( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::isAlpha( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].isAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDefault::getFilename( std::size_t _frame ) const
	{
		return m_vectorImageDescs[ _frame ].fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDefault::getCodecType( std::size_t _frame ) const
	{
		return m_vectorImageDescs[ _frame ].codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageDefault::getFilenameCount() const
	{
		//неверно, есть еще CreateImage
		return m_vectorImageDescs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::isValid() const
	{
		const ConstString & category = this->getCategory();

		for( TVectorImageDesc::const_iterator
			it = m_vectorImageDescs.begin(),
			it_end = m_vectorImageDescs.end();
		it != it_end;
		++it )
		{
			bool exist = this->validImageFrame_( category, it->fileName, it->codecType );

			if( exist == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::loader( BinParser * _parser )
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
				desc.wrapX = false;
				desc.wrapY = false;
				desc.isCombined = false;
				ConstString fileName;
				ConstString fileNameAlpha;
				ConstString fileNameRGB;

				String format;
				int from = -1;
				int to = -1;
				int step = 1;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::File_Alpha, desc.isAlpha );
					
					BIN_CASE_ATTRIBUTE( Protocol::File_PathAlpha, fileNameAlpha );
					BIN_CASE_ATTRIBUTE( Protocol::File_PathRGB, fileNameRGB );
					BIN_CASE_ATTRIBUTE( Protocol::File_isCombined, desc.isCombined );

					BIN_CASE_ATTRIBUTE( Protocol::File_Codec, desc.codecType );
					BIN_CASE_ATTRIBUTE( Protocol::File_UV, desc.uv );
					BIN_CASE_ATTRIBUTE( Protocol::File_Offset, desc.offset );
					BIN_CASE_ATTRIBUTE( Protocol::File_MaxSize, desc.maxSize );
					BIN_CASE_ATTRIBUTE( Protocol::File_Size, desc.size );
					BIN_CASE_ATTRIBUTE( Protocol::File_Path, fileName );
					
					
					BIN_CASE_ATTRIBUTE( Protocol::File_From, from );
					BIN_CASE_ATTRIBUTE( Protocol::File_To, to );
					BIN_CASE_ATTRIBUTE( Protocol::File_Step, step );
					BIN_CASE_ATTRIBUTE( Protocol::File_WrapX, desc.wrapX );
					BIN_CASE_ATTRIBUTE( Protocol::File_WrapY, desc.wrapY );
				}

				if( desc.isCombined == true )
				{
					desc.codecType = s_getImageCodec( fileNameAlpha );
					desc.codecTypeAlpha = s_getImageCodec( fileNameAlpha );
					desc.codecTypeRGB = s_getImageCodec( fileNameRGB );
					desc.fileNameAlpha = fileNameAlpha;
					desc.fileNameRGB = fileNameRGB;
				}
				
				
				//else
				//{
				if( fileName.empty() )
				{				
					if( ! fileNameRGB.empty() )
					{
						fileName = fileNameRGB;
					}

					if( ! fileNameAlpha.empty() )
					{
						fileName = fileNameAlpha;
					}
				}

				if( desc.codecType.empty() )
				{
					desc.codecType = s_getImageCodec( fileName );
				}
					
					//desc.isCombined = false;
				//}
				if( from >= 0 && to >= 0 )
				{
					char* fname = new char[fileName.size() * 2];
					if( step > 0 )
					{
						for( int i = from; i <= to; i += step )
						{
							sprintf( fname, fileName.c_str(), i );

							desc.fileName = ConstString(fname);

							m_vectorImageDescs.push_back( desc );
						}
					}
					else if( step < 0 )
					{
						for( int i = from; i >= to; i += step )
						{
							sprintf( fname, fileName.c_str(), i );

							desc.fileName = ConstString(fname);

							m_vectorImageDescs.push_back( desc );
						}
					}
					delete[] fname;
				}
				else
				{
					desc.fileName = fileName;
					m_vectorImageDescs.push_back( desc );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		int i = 0;

		for( TVectorImageDesc::iterator
			it = m_vectorImageDescs.begin(),
			it_end = m_vectorImageDescs.end();
		it != it_end;
		++it)
		{
			ImageFrame frame;

			if( it->fileName == Consts::get()->c_CreateImage )
			{
				const ConstString & name = getName();

				String createImageName = Helper::to_str(name) + Utils::toString( i++ );

				ConstString c_createImageName(createImageName);
				if( createImageFrame_( frame, c_createImageName, it->size ) == false )
				{
					return false;
				}
			}
			//else if( it->fileName == Consts::get()->c_CreateTarget )
			//{
			//	const ConstString & name = getName();

			//	if( createRenderTargetFrame_( frame, name, it->size ) == false )
			//	{
			//		return false;
			//	}
			//}
			else
			{
				const ConstString & category = this->getCategory();

				if( it->codecType.empty() == true )
				{
					it->codecType = s_getImageCodec( it->fileName );
				}

				if( it->isCombined == false )
				{
					if( this->loadImageFrame_( frame, category, it->fileName, it->codecType ) == false )
					{
						return false;
					}
				}
				else 
				{
					if( this->loadImageFrameCombineRGBAndAlpha_( frame, category, it->fileNameRGB, it->fileNameAlpha, it->codecTypeRGB, it->codecTypeAlpha ) == false )
					{
						return false;
					}
				}
				
				
			}

			frame.uv = it->uv;
			frame.uv_image = it->uv;
			
			frame.maxSize = it->maxSize;
			frame.offset =  it->offset;

			float ku = frame.uv.z - frame.uv.x;
			float kv = frame.uv.w - frame.uv.y;
			frame.size.x *= ku;
			frame.size.y *= kv;

			//frame.size.x = ::floorf( frame.size.x + 0.5f );
			//frame.size.y = ::floorf( frame.size.y + 0.5f );
			//mt::vec2f(frame.size.x * ku , frame.size.y * kv );

			frame.uv.x *= frame.pow_scale.x;
			frame.uv.y *= frame.pow_scale.y;
			frame.uv.z *= frame.pow_scale.x;
			frame.uv.w *= frame.pow_scale.y;

			if( frame.maxSize.x < 0.f || frame.maxSize.y < 0.f )
			{
				frame.maxSize = frame.size;
			}
			/*else
			{
				frame.size = it->size;
			}*/

			frame.isAlpha = it->isAlpha;
			frame.wrapX = it->wrapX;
			frame.wrapY = it->wrapY;

			m_vectorImageFrames.push_back( frame );
		}

		if( m_vectorImageFrames.empty() == true )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::_release()
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
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::addImagePath( const ConstString& _imagePath, const mt::vec2f & _size )
	{
		ImageDesc desc;
		desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
		desc.offset = mt::vec2f(0.f,0.f);
		desc.maxSize = mt::vec2f(-1.f,-1.f);
		desc.size = _size;
		//desc.isAlpha = false; //
		desc.isAlpha = true; //
		desc.wrapX = false;
		desc.wrapY = false;
		desc.fileName = _imagePath;
		desc.isCombined = false;

		m_vectorImageDescs.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::getWrapX( std::size_t _frame ) const 
	{
		return m_vectorImageFrames[ _frame ].wrapX;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::getWrapY( std::size_t _frame ) const 
	{
		return m_vectorImageFrames[ _frame ].wrapY;
	}
	/////////////////////////////////////////////////////////////////////////
}
