#	include "ResourceFont.h"

#	include "ResourceImplement.h"
#	include "ResourceImage.h"
#	include "ResourceGlyph.h"

#	include "BinParser.h"

#	include "ResourceManager.h"
#	include "FileEngine.h"

#	include "LogEngine.h"
#	include "Core/String.h"

#	include "RenderEngine.h"

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
	const RenderTextureInterface * ResourceFont::getImage() const
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderTextureInterface * ResourceFont::getOutlineImage() const
	{
		return m_outline;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceFont::getInitSize() const
	{
		if ( !m_glyph )
			return 0.0f;

		return m_glyph->getInitSize();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setImagePath_( const WString& _path )
	{
		m_imageFile = _path;

		m_imageCodec = ResourceImage::s_getImageCodec( m_imageFile );
	}
	//////////////////////////////////////////////////////////////////////////
	const WString& ResourceFont::getImagePath() const
	{
		return m_imageFile;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setOutlineImagePath_( const WString& _path )
	{
		m_outlineImageFile = _path;

		m_outlineImageCodec = ResourceImage::s_getImageCodec( m_outlineImageFile );
	}
	//////////////////////////////////////////////////////////////////////////
	const WString& ResourceFont::getOutlineImagePath() const
	{
		return m_outlineImageFile;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec4f ResourceFont::getUV( unsigned int _id ) const
	{
		if( this->isCompile() == false )
		{
			return mt::zero_v4;
		}

		const ResourceGlyph::Glyph * glyph = m_glyph->getGlyph( _id );

		if( glyph == 0 )
		{
			return mt::zero_v4;
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
		if( this->isCompile() == false )
		{
			return m_whsRatio;
		}

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
		if( this->isCompile() == false )
		{
			return mt::zero_v2;
		}

		const ResourceGlyph::Glyph * glyph = m_glyph->getGlyph( _id );

		if( glyph == 0 )
		{
			return mt::zero_v2;
		}

		return glyph->offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceFont::getSize( unsigned int _id ) const
	{
		if( this->isCompile() == false )
		{
			return mt::zero_v2;
		}

		const ResourceGlyph::Glyph * glyph = m_glyph->getGlyph( _id );

		if( glyph == 0 )
		{
			return mt::zero_v2;
		}

		return glyph->size;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & ResourceFont::getColor() const
	{
		return m_color;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			//XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceFont::setFontPath );
			BIN_CASE_ATTRIBUTE( Protocol::ResourceGlyph_Name, m_glyphResource );
			BIN_CASE_ATTRIBUTE( Protocol::Color_Value, m_color );
			
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
			MENGE_LOG_ERROR( "ResourceFont::_compile '%s' invalid loading font image '%S'"
				, this->getName().c_str()
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
				MENGE_LOG_ERROR( "ResourceFont::_compile '%s' can't loaded outline image file '%S'"
					, this->getName().c_str()
					, m_outlineImageFile.c_str() 
					);

				return false;
			}
		}

		if( m_glyphResource.empty() == true )
		{
			MENGE_LOG_ERROR("ResourceFont::_compile '%s' can't setup resource glyph"
				, this->getName().c_str()
				);

			return false;
		}
	
		m_glyph = ResourceManager::get()
			->getResourceT<ResourceGlyph>( m_glyphResource );

		if( m_glyph == 0 )
		{
			MENGE_LOG_ERROR("ResourceFont::_compile '%s' can't get resource glyph '%s'"
				, this->getName().c_str()
				, m_glyphResource.c_str()
				);

			return false;
		}

		if( m_glyph->compile() == false )
		{
			MENGE_LOG_ERROR("ResourceFont::_compile '%s' can't compile glyph '%s'"
				, this->getName().c_str()
				, m_glyphResource.c_str()
				);

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
			m_glyph->decrementReference();
			m_glyph = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
