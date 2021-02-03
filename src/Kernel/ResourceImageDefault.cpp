#include "ResourceImageDefault.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstString.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefault::ResourceImageDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefault::~ResourceImageDefault()
    {
        MENGINE_ASSERTION_FATAL( m_texture == nullptr );
        MENGINE_ASSERTION_FATAL( m_textureAlpha == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefault::_compile()
    {
        if( ResourceImage::_compile() == false )
        {
            LOGGER_ERROR( "'%s' invalid compile base ResourceImage class"
                , this->getName().c_str()
            );

            return false;
        }

        LOGGER_INFO( "image", "name '%s' load texture '%s'"
            , this->getName().c_str()
            , this->getFilePath().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "name '%s' group '%s' file path '%s' invalid setup category"
            , this->getName().c_str()
            , this->getGroupName().c_str()
            , this->getFilePath().c_str()
        );

        uint32_t decoder_options = DF_NONE;

#ifndef MENGINE_MASTER_RELEASE
        bool Engine_ForcePremultiplyAlpha = CONFIG_VALUE( "Engine", "ForcePremultiplyAlpha", false );

        if( Engine_ForcePremultiplyAlpha == true )
        {
            bool premultiply = this->isPremultiply();

            if( premultiply == false )
            {
                decoder_options = DF_PREMULTIPLY_ALPHA;

                this->setPremultiply( true );
            }
        }
#endif

        const FilePath & filePath = this->getFilePath();
        const ConstString & codecType = this->getCodecType();

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->loadTexture( fileGroup, filePath, codecType, decoder_options, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( texture, "name '%s' category '%s' group '%s' can't load image file '%s'"
            , this->getName().c_str()
            , this->getFileGroup()->getName().c_str()
            , this->getGroupName().c_str()
            , this->getFilePath().c_str()
        );

        m_texture = texture;

        this->prepareImageFrame_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::_release()
    {
        ResourceImage::_release();

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefault::_initialize()
    {
        this->correctUVTexture();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::prepareImageFrame_()
    {
        const RenderImageInterfacePtr & image = m_texture->getImage();

        uint32_t hwChannels = image->getHWChannels();

        if( hwChannels == 3 )
        {
            m_hasAlpha = false;
        }
        else
        {
            m_hasAlpha = true;
        }

        bool pow2 = m_texture->isPow2();

        this->setPow2( pow2 );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::correctUVTexture()
    {
        uint32_t width = (uint32_t)(m_size.x + 0.5f);
        uint32_t height = (uint32_t)(m_size.y + 0.5f);

        if( Helper::isTexturePOW2( width ) == false )
        {
            uint32_t width_pow2 = Helper::getTexturePOW2( width );

            float scale = m_size.x / float( width_pow2 );

            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].x = m_uvImage[i].x * scale;
                m_uvTextureAlpha[i].x = m_uvAlpha[i].x * scale;
            }
        }
        else
        {
            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].x = m_uvImage[i].x;
                m_uvTextureAlpha[i].x = m_uvAlpha[i].x;
            }
        }

        if( Helper::isTexturePOW2( height ) == false )
        {
            uint32_t height_pow2 = Helper::getTexturePOW2( height );

            float scale = m_size.y / float( height_pow2 );

            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].y = m_uvImage[i].y * scale;
                m_uvTextureAlpha[i].y = m_uvAlpha[i].y * scale;
            }
        }
        else
        {
            for( uint32_t i = 0; i != 4; ++i )
            {
                m_uvTextureImage[i].y = m_uvImage[i].y;
                m_uvTextureAlpha[i].y = m_uvAlpha[i].y;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
