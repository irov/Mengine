//#	include "ResourceMaterial.h"
//#	include "ResourceImplement.h"
//
//#	include "ResourceManager.h"
//#	include "ResourceImage.h"
//
//#	include "LoaderEngine.h"
//#	include "BinParser.h"
//
//#	include "LogEngine.h"
//
//namespace Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	RESOURCE_IMPLEMENT( ResourceMaterial );
//	//////////////////////////////////////////////////////////////////////////
//	ResourceMaterial::ResourceMaterial()
//		: m_resourceImage(0)
//		, m_color(1.0f, 1.0f, 1.0f, 1.0f)
//	{
//		mt::ident_m4( m_textureMatrix );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceMaterial::loader( BinParser * _parser )
//	{
//		ResourceReference::loader( _parser );
//		BIN_SWITCH_ID( _parser )
//		{
//			BIN_CASE_ATTRIBUTE( Protocol::File_Path, m_filename );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool ResourceMaterial::_compile()
//	{
//		const ConstString & category = this->getCategory();
//
//		if( XmlEngine::get()->
//			parseXmlFileM( category, m_filename, this, &ResourceMaterial::loaderMaterial_ ) == false )
//		{
//			MENGE_LOG_WARNING( "Warning: Parse material xml failed '%s'. Can't compile material"
//				, m_filename.c_str() 
//				);
//		}
//
//		//m_material = TMaterial();
//
//		if( m_textureName.empty() == false )
//		{
//			m_resourceImage = ResourceManager::get()->
//				getResourceT<ResourceImage>( m_textureName );
//				
//			//m_textureMatrix.m[12] = -m_resourceImage->getUV
//
//			if(m_resourceImage != NULL)
//			{
//				//m_material.texture = const_cast<RenderImageInterface*>( m_resourceImage->getImage( 0 ) );
//			}
//		}
//
//		//m_material.color = m_color.getAsARGB();
//
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceMaterial::_release()
//	{
//		if( m_resourceImage )
//		{
//			ResourceManager::get()
//				->releaseResource( m_resourceImage );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	const String & ResourceMaterial::getFilename() const
//	{
//		return m_filename;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	/*const TMaterial& ResourceMaterial::getMaterial() const
//	{
//		return m_material;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	TMaterial ResourceMaterial::getMaterial()
//	{
//		return m_material;
//	}*/
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceMaterial::loaderMaterial_( BinParser * _parser )
//	{
//		BIN_SWITCH_ID( _parser )
//		{
//			BIN_CASE_ATTRIBUTE( Protocol::Texture_Name, m_textureName );
//			BIN_CASE_ATTRIBUTE( Protocol::Color_Value, m_color );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//}	// namespace Menge
