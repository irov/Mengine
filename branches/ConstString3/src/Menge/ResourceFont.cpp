#	include "ResourceFont.h"

#	include "ResourceImplement.h"
#	include "ResourceImage.h"
#	include "ResourceGlyph.h"

#	include "BinParser.h"

#	include "ResourceManager.h"
#	include "FileEngine.h"

#	include "Logger/Logger.h"
#	include "Core/String.h"

#	include "RenderEngine.h"
#	include "Texture.h"

#	include <cstdio>
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceFont );
	//////////////////////////////////////////////////////////////////////////
	ResourceFont::ResourceFont()
		: m_image(NULL)
		, m_outline(NULL)
		, m_whsRatio(3.0f)
		, m_textureRatio(1.0f)
		, m_glyph(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceFont::getImage()
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceFont::getOutlineImage()
	{
		return m_outline;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceFont::getInitSize() const
	{
		return m_initSize;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setImagePath_( const ConstString& _path )
	{
		m_imageFile = _path;

		m_imageCodec = ResourceImage::s_getImageCodec( m_imageFile.str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setOutlineImagePath_( const ConstString& _path )
	{
		m_outlineImageFile = _path;

		m_outlineImageCodec = ResourceImage::s_getImageCodec( m_outlineImageFile.str() );
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceFont::getImagePath() const
	{
		return m_imageFile;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceFont::getOutlineImagePath() const
	{
		return m_outlineImageFile;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec4f ResourceFont::getUV( unsigned int _id ) const
	{
		const ResourceGlyph::Glyph * glyph = m_glyph->getGlyph( _id );

		if( glyph == 0 )
		{
			return mt::vec4f::zero_v4;
		}

		mt::vec4f uv = glyph->uv;

		uv.x *= m_imageInvSize.x;
		uv.y *= m_imageInvSize.y;
		uv.z *= m_imageInvSize.x;
		uv.w *= m_imageInvSize.y;

		return uv;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceFont::getCharRatio( unsigned int _id ) const
	{
		const ResourceGlyph::Glyph * glyph = m_glyph->getGlyph( _id );

		if( glyph == 0 )
		{
			return m_whsRatio;
		}

		return glyph->ratio;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceFont::getOffset( unsigned int _id ) const
	{
		const ResourceGlyph::Glyph * glyph = m_glyph->getGlyph( _id );

		if( glyph == 0 )
		{
			return mt::vec2f::zero_v2;
		}

		return glyph->offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceFont::getSize( unsigned int _id ) const
	{
		const ResourceGlyph::Glyph * glyph = m_glyph->getGlyph( _id );

		if( glyph == 0 )
		{
			return mt::vec2f::zero_v2;
		}

		return glyph->size;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			//XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceFont::setFontPath );
			BIN_CASE_ATTRIBUTE( Protocol::Glyph_Resource, m_glyphResource );

			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Image_Path, &ResourceFont::setImagePath_ );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::OutlineImage_Path, &ResourceFont::setOutlineImagePath_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::_compile()
	{
		const ConstString & category = this->getCategory();

		m_image = RenderEngine::get()
					->loadTexture( category, m_imageFile, m_imageCodec );

		if( m_image == NULL )
		{
			MENGE_LOG_ERROR( "Error while loading font image '%s'"
				, m_imageFile.c_str() 
				);

			return false;
		}

		m_textureRatio = static_cast<float>( m_image->getWidth() ) / m_image->getHeight();

		m_imageInvSize.x = 1.0f / m_image->getWidth();
		m_imageInvSize.y = 1.0f / m_image->getHeight();

		if( m_outlineImageFile.empty() == false )
		{
			m_outline = RenderEngine::get()
				->loadTexture( category, m_outlineImageFile, m_outlineImageCodec );

			if( m_outline == 0 )
			{
				MENGE_LOG_ERROR( "Image can't loaded '%s'"
					, m_outlineImageFile.c_str() 
					);

				return false;
			}
		}

		if( m_glyphResource.empty() == true )
		{
			return false;
		}
	
		m_glyph = ResourceManager::get()
			->getResourceT<ResourceGlyph>( m_glyphResource );

		if( m_glyph == 0 )
		{
			return false;
		}

		if( m_glyph->compile() == false )
		{
			return false;
		}
	
		m_whsRatio = getCharRatio('A');

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::_release()
	{
		RenderEngine::get()
			->releaseTexture( m_image );

		m_image = NULL;

		if( m_outline )
		{
			RenderEngine::get()
				->releaseTexture( m_outline );

			m_outline = 0;
		}

		if( m_glyph )
		{
			ResourceManager::get()
				->releaseResource( m_glyph );

			m_glyph = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
