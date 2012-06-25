#	include "ResourceImplement.h"
#	include "ResourceImageInAtlas.h"
#	include "ResourceImageAtlas.h"
#	include "ResourceManager.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "BinParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageInAtlas );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageInAtlas::ResourceImageInAtlas()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* ResourceImageInAtlas::getTexture() const
	{
		return m_resourceAtlas->getTexture();
		//return m_imageFrame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageInAtlas::loader( BinParser * _parser )
	{
		ResourceImage::loader( _parser );
		
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::ResourceAtlas_Name, m_resourceAtlasName );
			
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

				WString fileNameAlpha;
				WString fileNameRGB;
				WString fileName;

				String format;
				int from = -1;
				int to = -1;
				int step = 1;
				bool isCombined;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::File_Alpha, desc.isAlpha );

					BIN_CASE_ATTRIBUTE( Protocol::File_PathAlpha, fileNameAlpha );
					BIN_CASE_ATTRIBUTE( Protocol::File_PathRGB, fileNameRGB );
					BIN_CASE_ATTRIBUTE( Protocol::File_isCombined, isCombined );

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

				desc.fileNameAlpha = fileNameAlpha;
				desc.fileNameRGB = fileNameRGB;

				if( desc.fileNameAlpha.empty() == false )
				{
					desc.codecTypeAlpha = s_getImageCodec( desc.fileNameAlpha );
				}
				
				desc.codecTypeRGB = s_getImageCodec( desc.fileNameRGB );

				m_imageDesc = desc;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageInAtlas::_compile()
	{	
		if( m_resourceAtlasName.empty() == true )
		{
			MENGE_LOG_ERROR( "ResourceImageInAtlas: '%s' ImageAtlas resource empty"
				, m_name.c_str() 
				);

			return false;
		}
		
		m_resourceAtlas = ResourceManager::get()
			->getResourceT<ResourceImageAtlas>( m_resourceAtlasName );
		
		if( m_resourceAtlas == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageInAtlas: '%s' ImageAtlas resource not found '%s'"
				, m_name.c_str()
				, m_resourceAtlasName.c_str() 
				);

			return false;
		}
				
		m_imageFrame.uv = m_imageDesc.uv;

		m_imageFrame.maxSize = m_imageDesc.maxSize;
		m_imageFrame.offset =  m_imageDesc.offset;

		if( m_imageFrame.maxSize.x < 0.f || m_imageFrame.maxSize.y < 0.f )
		{
			m_imageFrame.maxSize = m_imageFrame.size;
		}

		m_imageFrame.wrapX = m_imageDesc.wrapX;
		m_imageFrame.wrapY = m_imageDesc.wrapY;
		
		const ConstString & category = this->getCategory();
		
		bool result = m_resourceAtlas->setImageInAtlas( this, m_imageFrame );

		if( result == false )
		{
			MENGE_LOG_ERROR("Texture::ResourceImageInAtlas: Invalid fill atlas texture"
				);

			return false;
		}
		
		return true;
	}
	/////////////////////////////////////////////////////////////////////////
	bool ResourceImageInAtlas::loadFrameData( unsigned char * _buffer, int _pitch )
	{
		if( m_imageDesc.fileNameRGB.empty() == true )
		{
			return false;
		}

		if( m_imageDesc.fileNameAlpha.empty() == true )
		{
			if ( this->loadFrameDataRGB_( _buffer, _pitch )  == false )
			{
				return false;
			}
		}
		else
		{
			if( this->loadFrameDataRGBAlpha_( _buffer, _pitch ) == false )
			{
				return false;
			}
		}

		return true;
	}
	/////////////////////////////////////////////////////////////////////////
	bool ResourceImageInAtlas::loadFrameDataRGBAlpha_( unsigned char * _buffer, int _pitch )
	{
		const ConstString & category = this->getCategory();

		////////////////////////////////////// init RGB Decoder
		FileInputStreamInterface * streamRGB = FileEngine::get()
			->openInputFile( category, m_imageDesc.fileNameRGB );

		if( streamRGB == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageCombineRGBAndAlpha::loadTextureCombineRGBAndAlpha: Image file with RGB data '%S' was not found"
				, m_imageDesc.fileNameRGB.c_str() 
				);

			return false;
		}

		ImageDecoderInterface * imageDecoderRGB = this->createDecoder_( streamRGB, m_imageDesc.codecTypeRGB );

		if( imageDecoderRGB == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, m_imageDesc.fileNameRGB.c_str() 
				);

			streamRGB->close();

			return false;
		}

		///Load Alpha data
		FileInputStreamInterface * streamAlpha = FileEngine::get()
			->openInputFile( category, m_imageDesc.fileNameAlpha );

		if( streamAlpha == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image file with alpha channel data '%s' was not found"
				, m_imageDesc.fileNameAlpha.c_str() 
				);

			streamRGB->close();
			imageDecoderRGB->destroy();

			return false;
		}

		///Get Alpha Decoder
		ImageDecoderInterface * imageDecoderAlpha = this->createDecoder_( streamAlpha, m_imageDesc.codecTypeAlpha );

		if( imageDecoderAlpha == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, m_imageDesc.fileNameAlpha.c_str() 
				);

			streamRGB->close();
			imageDecoderRGB->destroy();
			streamAlpha->close();

			return false;
		}

		this->loadRGBAndAlphaData_( _buffer, _pitch, imageDecoderRGB, imageDecoderAlpha );

		imageDecoderRGB->destroy();
		streamRGB->close();

		imageDecoderAlpha->destroy();
		streamAlpha->close();

		return true;
	}
	/////////////////////////////////////////////////////////////////////////
	bool ResourceImageInAtlas::loadFrameDataRGB_( unsigned char * _buffer, int _pitch )
	{
		const ConstString & category = this->getCategory();

		////////////////////////////////////// init RGB Decoder
		FileInputStreamInterface * streamRGB = FileEngine::get()
			->openInputFile( category, m_imageDesc.fileNameRGB );

		if( streamRGB == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageCombineRGBAndAlpha::loadTextureCombineRGBAndAlpha: Image file with RGB data '%S' was not found"
				, m_imageDesc.fileNameRGB.c_str() 
				);

			return false;
		}

		ImageDecoderInterface * imageDecoderRGB = this->createDecoder_( streamRGB, m_imageDesc.codecTypeRGB );

		if( imageDecoderRGB == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, m_imageDesc.fileNameRGB.c_str() 
				);

			streamRGB->close();

			return false;
		}

		ImageCodecOptions optionsRGB;
		optionsRGB.flags = DF_CUSTOM_PITCH;
		optionsRGB.pitch = _pitch;
		imageDecoderRGB->setOptions( &optionsRGB );

		RenderEngine::get()
			->loadBufferImageData( _buffer, _pitch, PF_X8R8G8B8, imageDecoderRGB );

		imageDecoderRGB->destroy();
		streamRGB->close();
		
		return true;
	}
	/////////////////////////////////////////////////////////////////////////
	void ResourceImageInAtlas::_release()
	{
		ResourceManager::get()
			->directResourceRelease( m_resourceAtlasName );
	}
	//////////////////////////////////////////////////////////////////////////
}
