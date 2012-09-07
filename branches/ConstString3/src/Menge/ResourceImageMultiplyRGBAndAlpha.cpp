#	include "ResourceImageMultiplyRGBAndAlpha.h"

#	include "ResourceImplement.h"
#	include "CodecEngine.h"
#	include "RenderEngine.h"
#	include "FileEngine.h"
#   include "LogEngine.h"

#	include "Interface/ImageCodecInterface.h"

#	include <cstdio>

#	include "Metacode.h"

#	include "Core/ConstString.h"
#	include "Utils/Core/String.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageMultiplyRGBAndAlpha );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageMultiplyRGBAndAlpha::ResourceImageMultiplyRGBAndAlpha()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    const WString & ResourceImageMultiplyRGBAndAlpha::getFileRGBName() const
    {
        return m_fileNameRGB;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageMultiplyRGBAndAlpha::getCodecRGBType() const
    {
        return m_codecTypeRGB;
    }
	//////////////////////////////////////////////////////////////////////////
	const WString & ResourceImageMultiplyRGBAndAlpha::getFileAlphaName() const
	{
		return m_fileNameAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageMultiplyRGBAndAlpha::getCodecAlphaType() const
	{
		return m_codecTypeAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageMultiplyRGBAndAlpha::isValid() const
	{
		const ConstString & category = this->getCategory();
		
		if( FileEngine::get()
			->existFile( category, m_fileNameRGB ) == false )
		{
			return false;
		}

		if( FileEngine::get()
			->existFile( category, m_fileNameAlpha ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageMultiplyRGBAndAlpha::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageMultiplyRGBAndAlpha * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageMultiplyRGBAndAlpha *>(_meta);

        m_uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_uv_alpha = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_maxSize = mt::vec2f(-1.f, -1.f);
        m_size = mt::vec2f(-1.f, -1.f);
        m_isAlpha = true;
        m_wrapX = false;
        m_wrapY = false;

        metadata->swap_FileRGB_Path( m_fileNameRGB );
        metadata->swap_FileRGB_Codec( m_codecTypeRGB );
        metadata->get_FileRGB_UV( m_uv );

        metadata->swap_FileAlpha_Path( m_fileNameAlpha );
        metadata->swap_FileAlpha_Codec( m_codecTypeAlpha );
        metadata->get_FileAlpha_UV( m_uv_alpha );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageMultiplyRGBAndAlpha::_compile()
	{	
        const ConstString & category = this->getCategory();

        if( m_codecTypeRGB.empty() == true )
        {
            m_codecTypeRGB = s_getImageCodec( m_fileNameRGB );
        }
        
        if( this->loadImageFrame_( category, m_fileNameRGB, m_codecTypeRGB ) == false )
        {
            return false;
        }

        if( m_codecTypeAlpha.empty() == true )
        {
            m_codecTypeAlpha = s_getImageCodec( m_fileNameAlpha );
        }
        	
        RenderTextureInterface * textureAlpha = RenderEngine::get()
            ->loadTexture( category, m_fileNameAlpha, m_codecTypeAlpha );

        if( textureAlpha == 0 )
        {
            MENGE_LOG_ERROR( "ResourceImageMultiplyRGBAndAlpha::_compile: '%s' can't load alpha image file '%S'"
                , this->getName().c_str()
                , m_fileNameAlpha.c_str()
                );

            return false;
        }

        m_textureAlpha = textureAlpha;

		return true;
	}	
	/////////////////////////////////////////////////////////////////////////
	bool ResourceImageMultiplyRGBAndAlpha::loadBufferAlpha( unsigned char * _buffer, int _pitch )
	{
		const ConstString & category = this->getCategory();

		///Load Alpha data
		FileInputStreamInterface * streamAlpha = FileEngine::get()
			->openInputFile( category, m_fileNameAlpha );

		if( streamAlpha == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageMultiplyRGBAndAlpha::loadBufferAlpha: Image file with alpha channel data '%s' was not found"
				, m_fileNameAlpha.c_str() 
				);

			return false;
		}

		///Get Alpha Decoder
		ImageDecoderInterface * imageDecoderAlpha = this->createDecoder_( streamAlpha, m_codecTypeAlpha );

		if( imageDecoderAlpha == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageMultiplyRGBAndAlpha::loadBufferAlpha: Image decoder for file '%s' was not found"
				, m_fileNameAlpha.c_str() 
				);

			streamAlpha->close();

			return false;
		}

        const ImageCodecDataInfo* dataInfoAlpha = imageDecoderAlpha->getCodecDataInfo();

        ImageCodecOptions optionsAlpha;

        optionsAlpha.flags |= DF_READ_ALPHA_ONLY;
        optionsAlpha.flags |= DF_CUSTOM_PITCH;
        optionsAlpha.pitch = _pitch;

        imageDecoderAlpha->setOptions( &optionsAlpha );

        const ImageCodecDataInfo * data = imageDecoderAlpha->getCodecDataInfo();

        unsigned int bufferSize = _pitch * data->height;
        unsigned int b = imageDecoderAlpha->decode( _buffer, bufferSize );

		imageDecoderAlpha->destroy();
		streamAlpha->close();

		return true;
	}
	/////////////////////////////////////////////////////////////////////////
}
