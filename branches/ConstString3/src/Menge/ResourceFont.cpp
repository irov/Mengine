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
		: m_texture(NULL)
		, m_outline(NULL)
		, m_whsRatio(3.0f)
		, m_textureRatio(1.0f)
		, m_resourceGlyph(NULL)
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
	bool ResourceFont::hasGlyph( unsigned int _id ) const
	{
		if( this->isCompile() == false )
		{
			return false;
		}

		bool isExist = m_resourceGlyph->hasGlyph( _id );

		return isExist;
	}
	//////////////////////////////////////////////////////////////////////////
	const Glyph * ResourceFont::getGlyph( unsigned int _id ) const
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
	void ResourceFont::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			//XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceFont::setFontPath );
			BIN_CASE_ATTRIBUTE( Protocol::ResourceGlyph_Name, m_resourceGlyphName );
			BIN_CASE_ATTRIBUTE( Protocol::Color_Value, m_color );
			
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Image_Path, &ResourceFont::setImagePath_ );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::OutlineImage_Path, &ResourceFont::setOutlineImagePath_ );
		}
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

		const Glyph * glyph = m_resourceGlyph->getGlyph('A');
	
		m_whsRatio = glyph->getRatio();

		return true;
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
