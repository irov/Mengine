#	include "ResourceImplement.h"
#	include "ResourceImageInAtlasDefault.h"
#	include "ResourceImageAtlas.h"
#	include "ResourceManager.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageInAtlasDefault );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageInAtlasDefault::ResourceImageInAtlasDefault()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* ResourceImageInAtlasDefault::getTexture() const
	{
		return m_resourceAtlas->getTexture();
		//return m_imageFrame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageInAtlasDefault::loader( const Metabuf::Metadata * _parser )
	{
		//ResourceImage::loader( _parser );

		//BIN_SWITCH_ID( _parser )
		//{
		//	BIN_CASE_ATTRIBUTE( Protocol::ResourceAtlas_Name, m_resourceAtlasName );

		//	BIN_CASE_NODE( Protocol::File )
		//	{
		//		ImageDesc desc;
		//		desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
		//		desc.offset = mt::vec2f(0.f,0.f);
		//		desc.maxSize = mt::vec2f(-1.f,-1.f);
		//		desc.size = mt::vec2f(-1.f,-1.f);
		//		desc.isAlpha = true; //
		//		desc.wrapX = false;
		//		desc.wrapY = false;

		//		WString fileNameAlpha;
		//		WString fileNameRGB;
		//		WString fileName;

		//		String format;
		//		int from = -1;
		//		int to = -1;
		//		int step = 1;
		//		bool isCombined;

		//		BIN_FOR_EACH_ATTRIBUTES()
		//		{
		//			BIN_CASE_ATTRIBUTE( Protocol::File_Alpha, desc.isAlpha );

		//			BIN_CASE_ATTRIBUTE( Protocol::File_PathAlpha, fileNameAlpha );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_PathRGB, fileNameRGB );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_isCombined, isCombined );

		//			BIN_CASE_ATTRIBUTE( Protocol::File_Codec, desc.codecType );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_UV, desc.uv );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_Offset, desc.offset );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_MaxSize, desc.maxSize );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_Size, desc.size );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_Path, fileName );


		//			BIN_CASE_ATTRIBUTE( Protocol::File_From, from );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_To, to );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_Step, step );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_WrapX, desc.wrapX );
		//			BIN_CASE_ATTRIBUTE( Protocol::File_WrapY, desc.wrapY );
		//		}

		//		desc.fileName = fileName;

		//		if( desc.codecType.empty() == true )
		//		{
		//			desc.codecType = s_getImageCodec( desc.fileName );
		//		}

		//		m_imageDesc = desc;
		//	}
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageInAtlasDefault::_compile()
	{	
		if( m_resourceAtlasName.empty() == true )
		{
			MENGE_LOG_ERROR( "ResourceImageInAtlasDefault: '%s' ImageAtlas resource empty"
				, m_name.c_str() 
				);

			return false;
		}

		m_resourceAtlas = ResourceManager::get()
			->getResourceT<ResourceImageAtlas>( m_resourceAtlasName );

		if( m_resourceAtlas == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageInAtlasDefault: '%s' ImageAtlas resource not found '%s'"
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
			MENGE_LOG_ERROR("Texture::ResourceImageInAtlasDefault: Invalid fill atlas texture"
				);

			return false;
		}

		return true;
	}
	/////////////////////////////////////////////////////////////////////////
	void ResourceImageInAtlasDefault::_release()
	{
		ResourceManager::get()
			->directResourceRelease( m_resourceAtlasName );
	}
	//////////////////////////////////////////////////////////////////////////
}
