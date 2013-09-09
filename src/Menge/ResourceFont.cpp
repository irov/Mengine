#	include "ResourceFont.h"

#	include "Kernel/ResourceImplement.h"
#	include "ResourceImage.h"
#	include "ResourceGlyph.h"

#	include "Interface/ResourceInterface.h"
#   include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"

#	include <cstdio>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceFont );
	//////////////////////////////////////////////////////////////////////////
	ResourceFont::ResourceFont()
		: m_textureRatio(1.0f)
		, m_resourceGlyph(nullptr)
		, m_textureUV(0, 0, 1.f, 1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceGlyph * ResourceFont::getResourceGlyph() const
	{
		return m_resourceGlyph;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderTextureInterfacePtr & ResourceFont::getTextureFont() const
	{
		return m_textureFont;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderTextureInterfacePtr & ResourceFont::getTextureOutline() const
	{
		return m_textureOutline;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setImagePath_( const FilePath& _path )
	{
		m_fontImageFile = _path;

        m_fontImageCodec = CODEC_SERVICE(m_serviceProvider)
            ->findCodecType( m_fontImageFile );
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath& ResourceFont::getImagePath() const
	{
		return m_fontImageFile;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setOutlineImagePath_( const FilePath& _path )
	{
		m_outlineImageFile = _path;

        m_outlineImageCodec = CODEC_SERVICE(m_serviceProvider)
            ->findCodecType( m_outlineImageFile );
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath& ResourceFont::getOutlineImagePath() const
	{
		return m_outlineImageFile;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::hasGlyph( GlyphChar _id, const Glyph ** _glyph ) const
	{
		if( this->isCompile() == false )
		{
			return false;
		}

		bool isExist = m_resourceGlyph->hasGlyph( _id, _glyph );

		return isExist;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& ResourceFont::getTextureUV() const
	{
		return m_textureUV;
	}
	//////////////////////////////////////////////////////////////////////////
	const Glyph * ResourceFont::getGlyph( GlyphChar _id ) const
	{
		if( this->isCompile() == false )
		{
			return nullptr;
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
	bool ResourceFont::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceFont * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceFont *>(_meta);

        metadata->swap_ResourceGlyph_Name( m_resourceGlyphName );
        metadata->swap_Image_Path( m_fontImageFile );
        
        if( metadata->swap_Image_Codec( m_fontImageCodec ) == false )
        {
            m_fontImageCodec = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_fontImageFile );
        }

        if( metadata->swap_OutlineImage_Path( m_outlineImageFile ) == true )
        {
            if( metadata->swap_OutlineImage_Codec( m_outlineImageCodec ) == false )
            {
                m_outlineImageCodec = CODEC_SERVICE(m_serviceProvider)
                    ->findCodecType( m_outlineImageFile );
            }
        }

        metadata->get_Color_Value( m_color );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::_compile()
	{
		const ConstString & category = this->getCategory();

		m_textureFont = RENDERTEXTURE_SERVICE(m_serviceProvider)
            ->loadTexture( category, m_fontImageFile, m_fontImageCodec, 0, 0 );

		//m_texture = RenderEngine::get()
		//	->loadMegatexture( category, m_imageFile, m_imageCodec );

		if( m_textureFont == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourceFont::_compile '%s' invalid loading font image '%s'"
				, this->getName().c_str()
				, m_fontImageFile.c_str() 
				);

			return false;
		}

		size_t width = m_textureFont->getWidth();
		size_t height = m_textureFont->getHeight();

		this->updateTextureUV_();

		m_textureRatio = float(width) / float(height);

		if( m_outlineImageFile.empty() == false )
		{
			m_textureOutline = RENDERTEXTURE_SERVICE(m_serviceProvider)
                ->loadTexture( category, m_outlineImageFile, m_outlineImageCodec, 0, 0 );

			if( m_textureOutline == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)( "ResourceFont::_compile '%s' can't loaded outline image file '%s'"
					, this->getName().c_str()
					, m_outlineImageFile.c_str() 
					);

				return false;
			}
		}

		if( m_resourceGlyphName.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceFont::_compile '%s' can't setup resource glyph"
				, this->getName().c_str()
				);

			return false;
		}
	
		m_resourceGlyph = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceGlyph>( m_resourceGlyphName );

		if( m_resourceGlyph == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceFont::_compile '%s' can't get resource glyph '%s'"
				, this->getName().c_str()
				, m_resourceGlyphName.c_str()
				);

			return false;
		}

        GlyphChar gc_A;
        gc_A.setCode('A');
		const Glyph * glyph = m_resourceGlyph->getGlyph(gc_A);
	
		if( glyph == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceFont::_compile: '%s' can't get from glyph '%s' - 'A'"
				, this->getName().c_str()
				, m_resourceGlyphName.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::updateTextureUV_()
	{
		size_t width = m_textureFont->getWidth();
		size_t height = m_textureFont->getHeight();

		size_t hwHeight = m_textureFont->getHWHeight();
		size_t hwWidth = m_textureFont->getHWWidth();

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
		m_textureFont = nullptr;
		m_textureOutline = nullptr;

		if( m_resourceGlyph != nullptr )
		{			
			m_resourceGlyph->decrementReference();
			m_resourceGlyph = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
