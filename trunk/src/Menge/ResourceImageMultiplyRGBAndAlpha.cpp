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
    const FilePath & ResourceImageMultiplyRGBAndAlpha::getFilePathRGB() const
    {
        return m_filePathRGB;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageMultiplyRGBAndAlpha::getCodecTypeRGB() const
    {
        return m_codecTypeRGB;
    }
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceImageMultiplyRGBAndAlpha::getFilePathAlpha() const
	{
		return m_filePathAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageMultiplyRGBAndAlpha::getCodecTypeAlpha() const
	{
		return m_codecTypeAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageMultiplyRGBAndAlpha::_isValid() const
	{
		const ConstString & category = this->getCategory();
		
		if( FILE_SERVICE(m_serviceProvider)
			->existFile( category, m_filePathRGB, nullptr ) == false )
		{
			return false;
		}

		if( FILE_SERVICE(m_serviceProvider)
			->existFile( category, m_filePathAlpha, nullptr ) == false )
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
        m_maxSize = mt::vec2f(0.f, 0.f);
        m_size = mt::vec2f(0.f, 0.f);
		m_offset = mt::vec2f(0.f, 0.f);
        m_isAlpha = true;
        m_wrapU = false;
        m_wrapV = false;

        metadata->get_File_WrapX( m_wrapU );
        metadata->get_File_WrapY( m_wrapV );

        metadata->swap_File_PathRGB( m_filePathRGB );
        metadata->swap_File_CodecRGB( m_codecTypeRGB );
        metadata->get_File_UVRGB( m_uv );

        metadata->swap_File_PathAlpha( m_filePathAlpha );
        metadata->swap_File_CodecAlpha( m_codecTypeAlpha );
        metadata->get_File_UVAlpha( m_uv_alpha );

		m_maxSize = metadata->get_File_MaxSize();

		m_size = m_maxSize;
		metadata->get_File_Size( m_size );
		metadata->get_File_Offset( m_offset );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageMultiplyRGBAndAlpha::_compile()
	{	
        const ConstString & category = this->getCategory();

        if( m_codecTypeRGB.empty() == true )
        {
            m_codecTypeRGB = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_filePathRGB );
        }
        
        if( this->loadImageFrame_( category, m_filePathRGB, m_codecTypeRGB ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("ResourceImageMultiplyRGBAndAlpha::_compile: '%s' can't load rgb image file '%s' codec '%s'"
				, this->getName().c_str()
				, m_filePathRGB.c_str()
				, m_codecTypeRGB.c_str()
				);

            return false;
        }

        if( m_codecTypeAlpha.empty() == true )
        {
            m_codecTypeAlpha = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_filePathAlpha );
        }
        	
        RenderTextureInterfacePtr textureAlpha = RENDERTEXTURE_SERVICE(m_serviceProvider)
            ->loadTexture( category, m_filePathAlpha, m_codecTypeAlpha );

        if( textureAlpha == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceImageMultiplyRGBAndAlpha::_compile: '%s' can't load alpha image file '%s' codec '%s'"
                , this->getName().c_str()
                , m_filePathAlpha.c_str()
				, m_codecTypeAlpha.c_str()
                );

            return false;
        }

        m_textureAlpha = textureAlpha;

		return true;
	}	
	/////////////////////////////////////////////////////////////////////////
}
