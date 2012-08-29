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
	const WString & ResourceImageDefault::getFileName() const
	{
		return m_fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDefault::getCodecType() const
	{
		return m_codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::isValid() const
	{
		const ConstString & category = this->getCategory();

		bool exist = FileEngine::get()
			->existFile( category, m_fileName );
				
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

        m_imageFrame.uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_imageFrame.maxSize = mt::vec2f(-1.f, -1.f);
        m_imageFrame.size = mt::vec2f(-1.f, -1.f);
        m_imageFrame.isAlpha = true;
        m_imageFrame.wrapX = false;
        m_imageFrame.wrapY = false;
        
        metadata->swap_File_Path( m_fileName );        
        metadata->swap_File_Codec( m_codecType );        

        //metadata->get_File_MaxSize( m_imageFrame.maxSize );
        metadata->get_File_UV( m_imageFrame.uv );
        //metadata->get_File_Size( m_imageDesc.size );
        metadata->get_File_Alpha( m_imageFrame.isAlpha );
        metadata->get_File_WrapX( m_imageFrame.wrapX );
        metadata->get_File_WrapY( m_imageFrame.wrapY );
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		const ConstString & category = this->getCategory();

		if( m_codecType.empty() == true )
		{
			m_codecType = s_getImageCodec( m_fileName );
		}
			
		if( this->loadImageFrame_( m_imageFrame, category, m_fileName, m_codecType ) == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::_release()
	{
		this->releaseImageFrame_( m_imageFrame );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::setImagePath( const WString& _imagePath )
	{
        m_fileName = _imagePath;
        m_codecType = s_getImageCodec(_imagePath);

        m_imageFrame.texture = NULL;
        m_imageFrame.uv = mt::vec4f(0.f,0.f,1.f,1.f);
		m_imageFrame.isAlpha = true;
		m_imageFrame.wrapX = false;
		m_imageFrame.wrapY = false;
	}
	/////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::loadBufferAlpha( unsigned char * _buffer, int _pitch )
	{
		const ConstString & category = this->getCategory();

		FileInputStreamInterface * stream = FileEngine::get()
			->openInputFile( category, m_fileName );

		if( stream == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageCombineRGBAndAlpha::loadTextureCombineRGBAndAlpha: Image file with RGB data '%S' was not found"
				, m_fileName.c_str() 
				);

			return false;
		}

		ImageDecoderInterface * imageDecoder = this->createDecoder_( stream, m_codecType );

		if( imageDecoder == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, m_fileName.c_str() 
				);

			stream->close();

			return false;
		}

        ImageCodecOptions options;
        options.flags |= DF_READ_ALPHA_ONLY;

        options.pitch = _pitch;
        options.flags |= DF_CUSTOM_PITCH;

		imageDecoder->setOptions( &options );
        
        const ImageCodecDataInfo * data = imageDecoder->getCodecDataInfo();

        unsigned int bufferSize = _pitch * data->height;
        imageDecoder->decode( _buffer, bufferSize );

		imageDecoder->destroy();

		stream->close();

		return true;
	}
	/////////////////////////////////////////////////////////////////////////
}
