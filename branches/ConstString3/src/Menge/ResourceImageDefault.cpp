#	include "ResourceImageDefault.h"

#	include "ResourceImplement.h"

#	include "FileEngine.h"

#   include "LogEngine.h"
#   include "Metacode.h"

#	include "Core/ConstString.h"
#	include "Utils/Core/String.h"
#	include "RenderEngine.h"
#	include "Consts.h"
#	include "Interface/ImageCodecInterface.h"

#	include <cstdio>

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
	const WString & ResourceImageDefault::getFileName() const
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

		bool exist = FileEngine::get()
			->existFile( category, m_imageDesc.fileName );
				
		if( exist == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageDefault * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageDefault *>(_meta);

        m_imageDesc.uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_imageDesc.maxSize = mt::vec2f(-1.f, -1.f);
        m_imageDesc.offset = mt::vec2f(0.f, 0.f);
        m_imageDesc.size = mt::vec2f(-1.f, -1.f);
        m_imageDesc.isAlpha = true;
        m_imageDesc.wrapX = false;
        m_imageDesc.wrapY = false;
        
        metadata->swap_File_Path( m_imageDesc.fileName );
        
        if( metadata->swap_File_Codec( m_imageDesc.codecType ) == false )
        {
            m_imageDesc.codecType = s_getImageCodec( m_imageDesc.fileName );
        }

        metadata->get_File_MaxSize( m_imageDesc.maxSize );
        metadata->get_File_UV( m_imageDesc.uv );
        metadata->get_File_Alpha( m_imageDesc.isAlpha );
        metadata->get_File_WrapX( m_imageDesc.wrapX );
        metadata->get_File_WrapY( m_imageDesc.wrapY );
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		ImageFrame frame;

		const ConstString & category = this->getCategory();

		if( m_imageDesc.codecType.empty() == true )
		{
			m_imageDesc.codecType = s_getImageCodec( m_imageDesc.fileName );
		}
			
		if( this->loadImageFrame_( frame, category, m_imageDesc.fileName, m_imageDesc.codecType ) == false )
		{
			return false;
		}
		
		//frame.uv = m_imageDesc.uv;
		frame.uv_image = m_imageDesc.uv;

		frame.maxSize = m_imageDesc.maxSize;
		frame.offset =  m_imageDesc.offset;

		float ku = m_imageDesc.uv.z - m_imageDesc.uv.x;
		float kv = m_imageDesc.uv.w - m_imageDesc.uv.y;
		frame.size.x *= ku;
		frame.size.y *= kv;

		//frame.size.x = ::floorf( frame.size.x + 0.5f );
		//frame.size.y = ::floorf( frame.size.y + 0.5f );
		//mt::vec2f(frame.size.x * ku , frame.size.y * kv );

		frame.uv.x = frame.uv_scale.x + (frame.uv_scale.z - frame.uv_scale.x) * m_imageDesc.uv.x;
		frame.uv.y = frame.uv_scale.y + (frame.uv_scale.w - frame.uv_scale.y) * m_imageDesc.uv.y;
		frame.uv.z = frame.uv_scale.x + (frame.uv_scale.z - frame.uv_scale.x) * m_imageDesc.uv.z;
		frame.uv.w = frame.uv_scale.y + (frame.uv_scale.w - frame.uv_scale.y) * m_imageDesc.uv.w;

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
	void ResourceImageDefault::addImagePath( const WString& _imagePath, const mt::vec2f & _size )
	{
		ImageDesc desc;
		desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
		desc.offset = mt::vec2f(0.f,0.f);
		desc.maxSize = _size;
		desc.size = _size;
		//desc.isAlpha = false; //
		desc.isAlpha = true; //
		desc.wrapX = false;
		desc.wrapY = false;
		desc.fileName = _imagePath;
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
	bool ResourceImageDefault::loadBuffer( unsigned char * _buffer, int _pitch )
	{
		const ConstString & category = this->getCategory();

		////////////////////////////////////// init RGB Decoder
		FileInputStreamInterface * stream = FileEngine::get()
			->openInputFile( category, m_imageDesc.fileName );

		if( stream == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageCombineRGBAndAlpha::loadTextureCombineRGBAndAlpha: Image file with RGB data '%S' was not found"
				, m_imageDesc.fileName.c_str() 
				);

			return false;
		}

		ImageDecoderInterface * imageDecoder = this->createDecoder_( stream, m_imageDesc.codecType );

		if( imageDecoder == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, m_imageDesc.fileName.c_str() 
				);

			stream->close();

			return false;
		}

		ImageCodecOptions options;
		options.flags = DF_CUSTOM_PITCH;
		options.pitch = _pitch;
		imageDecoder->setOptions( &options );

		RenderEngine::get()
			->loadBufferImageData( _buffer, _pitch, PF_A8R8G8B8, imageDecoder );

		imageDecoder->destroy();
		stream->close();

		return true;
	}
	/////////////////////////////////////////////////////////////////////////
}
