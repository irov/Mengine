#	include "ResourceImplement.h"
#	include "ResourceImageInAtlas.h"
#	include "ResourceImageAtlas.h"
#	include "ResourceManager.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "LogEngine.h"

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
	const mt::vec2f & ResourceImageInAtlas::getMaxSize() const
	{
		return m_imageFrame.maxSize;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageInAtlas::getSize() const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageInAtlas::getOffset() const
	{
		return m_imageFrame.offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageInAtlas::getUV() const
	{
		return m_imageFrame.uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageInAtlas::getUVImage() const
	{
		return m_imageFrame.uv_image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* ResourceImageInAtlas::getTexture() const
	{
		return m_resourceAtlas->getTexture();
		//return m_imageFrame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageInAtlas::isAlpha() const
	{
		return m_imageFrame.isAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageInAtlas::getFileName() const
	{
		return m_imageDesc.fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageInAtlas::getCodecType() const
	{
		return m_imageDesc.codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageInAtlas::getWrapX() const 
	{
		return m_imageFrame.wrapX;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageInAtlas::getWrapY() const 
	{
		return m_imageFrame.wrapY;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageInAtlas::isValid() const
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
	void ResourceImageInAtlas::loader( BinParser * _parser )
	{
		ResourceImage::loader( _parser );
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
		
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::ResourceAtlas_Name, m_resourceAtlasName );
			BIN_CASE_NODE( Protocol::File )
			{
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
					BIN_CASE_ATTRIBUTE( Protocol::File_WrapX, desc.wrapX );
					BIN_CASE_ATTRIBUTE( Protocol::File_WrapY, desc.wrapY );
				}
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

		if( m_imageDesc.codecType.empty() == true )
		{
			m_imageDesc.codecType = s_getImageCodec( m_imageDesc.fileName );
		}

		if( m_imageDesc.isCombined == false )
		{
			FileInputStreamInterface * file = this->createStream_( category ,  m_imageDesc.fileName );
			
			if( file == NULL )
			{
				MENGE_LOG_ERROR( "ResourceImageInAtlas:: Image file '%s' was not found"
					, m_imageDesc.fileName.c_str() 
					);
				
				return false;
			}
			
			ImageDecoderInterface * decoder = this->createDecoder_( file, m_imageDesc.codecType );
			if( decoder == NULL )
			{
				MENGE_LOG_ERROR("ResourceImageInAtlas:: Can`t create Decoder for file %s codec %s", m_imageDesc.fileName.c_str(), m_imageDesc.codecType.c_str());
				file->close();
				return false;
			}
			
			bool result = m_resourceAtlas->loadFrame( decoder, m_imageFrame );
			if( result == false )
			{
				MENGE_LOG_ERROR("Texture::ResourceImageInAtlas: Invalid fill atlas texture");
				decoder->destroy();
				file->close();
				return false;
			}		
			decoder->destroy();
			file->close();
		}
		else 
		{
			/*if( this->loadImageFrameCombineRGBAndAlpha_( frame, category, m_imageDesc.fileNameRGB, m_imageDesc.fileNameAlpha, m_imageDesc.codecTypeRGB, m_imageDesc.codecTypeAlpha ) == false )
			{
				return false;
			}*/
			MENGE_LOG_ERROR("WE NOT SUPPORT RGB ALPHA NOW");
			return false;
		}		
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageInAtlas::_release()
	{
		ResourceManager::get()
			->directResourceRelease(m_resourceAtlasName);
	}
	/////////////////////////////////////////////////////////////////////////
}
