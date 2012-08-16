#	include "ResourceFont.h"

#	include "ResourceImplement.h"
#	include "ResourceImage.h"
#	include "ResourceGlyph.h"

#	include "Metacode.h"

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
		: m_texture(NULL)
		, m_outline(NULL)
		, m_whsRatio(3.0f)
		, m_textureRatio(1.0f)
		, m_resourceGlyph(NULL)
		, m_textureUV(0, 0, 1, 1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceGlyph * ResourceFont::getResourceGlyph() const
	{
		return m_resourceGlyph;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderTextureInterface * ResourceFont::getTexture() const
	{
		return m_texture;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderTextureInterface * ResourceFont::getTextureImage() const
	{
		return m_outline;
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
	bool ResourceFont::hasGlyph( wchar_t _id ) const
	{
		if( this->isCompile() == false )
		{
			return false;
		}

		bool isExist = m_resourceGlyph->hasGlyph( _id );

		return isExist;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& ResourceFont::getTextureUV() const
	{
		return m_textureUV;
	}
	//////////////////////////////////////////////////////////////////////////
	const Glyph * ResourceFont::getGlyph( wchar_t _id ) const
	{
		if( this->isCompile() == false )
		{
			return NULL;
		}

		const Glyph * glyph = m_resourceGlyph->getGlyph( _id );

		return glyph;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & ResourceFont::getColor() const
	{
		return m_color;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceFont * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceFont *>(_meta);

        metadata->swap_ResourceGlyph_Name( m_resourceGlyphName );
        metadata->swap_Image_Path( m_imageFile );
        
        if( metadata->swap_Image_Codec( m_imageCodec ) == false )
        {
            m_imageCodec = ResourceImage::s_getImageCodec( m_imageFile );
        }

        if( metadata->swap_OutlineImage_Path( m_outlineImageFile ) == true )
        {
            if( metadata->swap_OutlineImage_Codec( m_outlineImageCodec ) == false )
            {
                m_outlineImageCodec = ResourceImage::s_getImageCodec( m_outlineImageFile );
            }
        }

        metadata->get_Color_Value( m_color );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::_compile()
	{
		const ConstString & category = this->getCategory();

		m_texture = RenderEngine::get()
					->loadTexture( category, m_imageFile, m_imageCodec );

		//m_texture = RenderEngine::get()
		//	->loadMegatexture( category, m_imageFile, m_imageCodec );

		if( m_texture == NULL )
		{
			MENGE_LOG_ERROR( "ResourceFont::_compile '%s' invalid loading font image '%S'"
				, this->getName().c_str()
				, m_imageFile.c_str() 
				);

			return false;
		}

		size_t width = m_texture->getWidth();
		size_t height = m_texture->getHeight();

		this->updateTextureUV_();

		m_textureRatio = float(width) / float(height);

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

		if( m_resourceGlyphName.empty() == true )
		{
			MENGE_LOG_ERROR("ResourceFont::_compile '%s' can't setup resource glyph"
				, this->getName().c_str()
				);

			return false;
		}
	
		m_resourceGlyph = ResourceManager::get()
			->getResourceT<ResourceGlyph>( m_resourceGlyphName );

		if( m_resourceGlyph == 0 )
		{
			MENGE_LOG_ERROR("ResourceFont::_compile '%s' can't get resource glyph '%s'"
				, this->getName().c_str()
				, m_resourceGlyphName.c_str()
				);

			return false;
		}

		const Glyph * glyph = m_resourceGlyph->getGlyph(L'A');
	
		if( glyph == 0 )
		{
			MENGE_LOG_ERROR("ResourceFont::_compile: '%s' can't get from glyph '%s' - 'A'"
				, this->getName().c_str()
				, m_resourceGlyphName.c_str()
				);

			return false;
		}

		m_whsRatio = glyph->getRatio();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::updateTextureUV_()
	{
		size_t width = m_texture->getWidth();
		size_t height = m_texture->getHeight();

		size_t hwHeight = m_texture->getHWHeight();
		size_t hwWidth = m_texture->getHWWidth();

		float scaleRight = float(width) / float(hwWidth);
		float scaleBottom = float(height) / float(hwHeight);

		m_textureUV.x = 0;
		m_textureUV.y = 0;
		m_textureUV.z = scaleRight;
		m_textureUV.w = scaleBottom;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::_release()
	{
		RenderEngine::get()
			->releaseTexture( m_texture );

		m_texture = NULL;

		if( m_outline )
		{
			RenderEngine::get()
				->releaseTexture( m_outline );

			m_outline = 0;
		}

		if( m_resourceGlyph )
		{			
			m_resourceGlyph->decrementReference();
			m_resourceGlyph = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
