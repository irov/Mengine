
#	include "ResourceMaterial.h"
#	include "ResourceImplement.h"
#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "ResourceManager.h"
#	include "ResourceImage.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMaterial );
	//////////////////////////////////////////////////////////////////////////
	ResourceMaterial::ResourceMaterial( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
		, m_resourceImage( 0 )
		, m_color( 1.0f, 1.0f, 1.0f, 1.0f )
	{
		mt::ident_m4( m_textureMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMaterial::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("File"), MENGE_TEXT("Path"), m_filename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMaterial::_compile()
	{
		if( Holder<XmlEngine>::hostage()->
			parseXmlFileM( m_params.category + m_filename, this, &ResourceMaterial::loaderMaterial_ ) == false )
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Warning: Parse material xml failed '%s'. Can't compile material")
				, m_filename.c_str()
				);
		}

		m_material = TMaterial();

		if( m_textureName != MENGE_TEXT("") )
		{
			m_resourceImage = static_cast<ResourceImage*>( 
				Holder<ResourceManager>::hostage()->
				getResource( m_textureName ) 
				);
			//m_textureMatrix.m[12] = -m_resourceImage->getUV
			m_material.texture = const_cast<RenderImageInterface*>( m_resourceImage->getImage( 0 ) );
		}

		m_material.color = m_color.getAsARGB();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMaterial::_release()
	{
		if( m_resourceImage )
		{
			Holder<ResourceManager>::hostage()
				->releaseResource( m_resourceImage );
			m_material.texture = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const TMaterial& ResourceMaterial::getMaterial() const
	{
		return m_material;
	}
	//////////////////////////////////////////////////////////////////////////
	TMaterial ResourceMaterial::getMaterial()
	{
		return m_material;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMaterial::loaderMaterial_( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Texture"), MENGE_TEXT("Name"), m_textureName );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Color"), MENGE_TEXT("Value"), m_color );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge