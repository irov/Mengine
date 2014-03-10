#	include "ResourceImageMultiplyRGBAndAlpha.h"
#	include "Kernel/ResourceImplement.h"

#   include "Logger/Logger.h"

#	include "Interface/ImageCodecInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include <cstdio>

#	include "Core/ConstString.h"
#	include "Core/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageMultiplyRGBAndAlpha );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageMultiplyRGBAndAlpha::ResourceImageMultiplyRGBAndAlpha()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceImageMultiplyRGBAndAlpha::getFileRGBName() const
    {
        return m_fileNameRGB;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageMultiplyRGBAndAlpha::getCodecRGBType() const
    {
        return m_codecTypeRGB;
    }
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceImageMultiplyRGBAndAlpha::getFileAlphaName() const
	{
		return m_fileNameAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageMultiplyRGBAndAlpha::getCodecAlphaType() const
	{
		return m_codecTypeAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageMultiplyRGBAndAlpha::_isValid() const
	{
		const ConstString & category = this->getCategory();
		
		if( FILE_SERVICE(m_serviceProvider)
			->existFile( category, m_fileNameRGB, nullptr ) == false )
		{
			return false;
		}

		if( FILE_SERVICE(m_serviceProvider)
			->existFile( category, m_fileNameAlpha, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageMultiplyRGBAndAlpha::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageMultiplyRGBAndAlpha * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageMultiplyRGBAndAlpha *>(_meta);

        m_uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_uv_alpha = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_textureSize = mt::vec2f(0.f, 0.f);
        m_maxSize = mt::vec2f(0.f, 0.f);
        m_size = mt::vec2f(0.f, 0.f);
        m_isAlpha = true;
        m_wrapU = false;
        m_wrapV = false;

        metadata->get_File_WrapX( m_wrapU );
        metadata->get_File_WrapY( m_wrapV );

        metadata->swap_FileRGB_Path( m_fileNameRGB );
        metadata->swap_FileRGB_Codec( m_codecTypeRGB );
        metadata->get_FileRGB_UV( m_uv );

        metadata->swap_FileAlpha_Path( m_fileNameAlpha );
        metadata->swap_FileAlpha_Codec( m_codecTypeAlpha );
        metadata->get_FileAlpha_UV( m_uv_alpha );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageMultiplyRGBAndAlpha::_compile()
	{	
        const ConstString & category = this->getCategory();

        if( m_codecTypeRGB.empty() == true )
        {
            m_codecTypeRGB = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_fileNameRGB );
        }
        
        if( this->loadImageFrame_( category, m_fileNameRGB, m_codecTypeRGB ) == false )
        {
            return false;
        }

        m_isAlpha = true;

        if( m_codecTypeAlpha.empty() == true )
        {
            m_codecTypeAlpha = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_fileNameAlpha );
        }
        	
        RenderTextureInterfacePtr textureAlpha = RENDERTEXTURE_SERVICE(m_serviceProvider)
            ->loadTexture( category, m_fileNameAlpha, m_codecTypeAlpha );

        if( textureAlpha == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceImageMultiplyRGBAndAlpha::_compile: '%s' can't load alpha image file '%s'"
                , this->getName().c_str()
                , m_fileNameAlpha.c_str()
                );

            return false;
        }

        m_textureAlpha = textureAlpha;

		return true;
	}	
	/////////////////////////////////////////////////////////////////////////
}
