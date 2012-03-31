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
	const mt::vec2f & ResourceImageDefault::getMaxSize() const
	{
		return m_imageFrame.maxSize;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getSize() const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getOffset() const
	{
		return m_imageFrame.offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDefault::getUV() const
	{
		return m_imageFrame.uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDefault::getUVImage() const
	{
		return m_imageFrame.uv_image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* ResourceImageDefault::getTexture() const
	{
		return m_imageFrame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::isAlpha() const
	{
		return m_imageFrame.isAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDefault::getFileName() const
	{
		return m_imageDesc.fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDefault::getCodecType() const
	{
		return m_imageDesc.codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::isValid() const
	{
		const ConstString & category = this->getCategory();

		bool exist = this->validImageFrame_( category, m_imageDesc.fileName, m_imageDesc.codecType );

		if( exist == false )
		{
			return false;
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

				desc.fileName = fileName;
				m_imageDesc = desc;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		ImageFrame frame;

		if( m_imageDesc.fileName == Consts::get()->c_CreateImage )
		{
			const ConstString & name = getName();

			String createImageName = Helper::to_str(name);

			ConstString c_createImageName(createImageName);
			if( createImageFrame_( frame, c_createImageName, m_imageDesc.size ) == false )
			{
				return false;
			}
		}
		else
		{
			const ConstString & category = this->getCategory();

			if( m_imageDesc.codecType.empty() == true )
			{
				m_imageDesc.codecType = s_getImageCodec( m_imageDesc.fileName );
			}

			if( m_imageDesc.isCombined == false )
			{
				if( this->loadImageFrame_( frame, category, m_imageDesc.fileName, m_imageDesc.codecType ) == false )
				{
					return false;
				}
			}
			else 
			{
				if( this->loadImageFrameCombineRGBAndAlpha_( frame, category, m_imageDesc.fileNameRGB, m_imageDesc.fileNameAlpha, m_imageDesc.codecTypeRGB, m_imageDesc.codecTypeAlpha ) == false )
				{
					return false;
				}
			}		
		}

		frame.uv = m_imageDesc.uv;
		frame.uv_image = m_imageDesc.uv;

		frame.maxSize = m_imageDesc.maxSize;
		frame.offset =  m_imageDesc.offset;

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

		frame.isAlpha = m_imageDesc.isAlpha;
		frame.wrapX = m_imageDesc.wrapX;
		frame.wrapY = m_imageDesc.wrapY;

		m_imageFrame = frame;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::_release()
	{
		this->releaseImageFrame_( m_imageFrame );
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
		desc.codecType = s_getImageCodec(_imagePath);

		m_imageDesc = desc;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::getWrapX() const 
	{
		return m_imageFrame.wrapX;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::getWrapY() const 
	{
		return m_imageFrame.wrapY;
	}
	/////////////////////////////////////////////////////////////////////////
}
