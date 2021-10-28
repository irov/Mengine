#include "ResourceImageDefault.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstString.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/ConfigHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefault::ResourceImageDefault()
#ifndef MENGINE_MASTER_RELEASE
        : m_forcePremultiply( false )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefault::~ResourceImageDefault()
    {
        MENGINE_ASSERTION_FATAL( this->getTexture() == nullptr );
        MENGINE_ASSERTION_FATAL( this->getTextureAlpha() == nullptr );
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
            , this->getContent()->getFilePath().c_str()
        );

        const ContentInterfacePtr & content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "name '%s' group '%s' file path '%s' invalid setup category"
            , this->getName().c_str()
            , this->getGroupName().c_str()
            , this->getContent()->getFilePath().c_str()
        );

        uint32_t decoder_options = DF_IMAGE_NONE;

#ifndef MENGINE_MASTER_RELEASE
        bool Engine_ForcePremultiplyAlpha = CONFIG_VALUE( "Engine", "ForcePremultiplyAlpha", false );

        if( Engine_ForcePremultiplyAlpha == true )
        {
            //ToDo: fix required setup alpha
            //bool alpha = this->hasAlpha();
            bool premultiply = this->isPremultiply();

            if( premultiply == false /*&& alpha == true*/ )
            {
                decoder_options |= DF_IMAGE_PREMULTIPLY_ALPHA;

                this->setPremultiply( true );

                m_forcePremultiply = true;
            }
        }
#endif

        const FilePath & filePath = m_content->getFilePath();
        const ConstString & codecType = m_content->getCodecType();

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->loadTexture( fileGroup, filePath, codecType, decoder_options, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( texture, "name '%s' category '%s' group '%s' can't load image file '%s'"
            , this->getName().c_str()
            , this->getContent()->getFileGroup()->getName().c_str()
            , this->getGroupName().c_str()
            , this->getContent()->getFilePath().c_str()
        );

        this->setTexture( texture );

        this->prepareImageFrame_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::_release()
    {
        ResourceImage::_release();

#ifndef MENGINE_MASTER_RELEASE
        if( m_forcePremultiply == true )
        {
            this->setPremultiply( false );

            m_forcePremultiply = false;
        }
#endif

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
        const RenderTextureInterfacePtr & texture = this->getTexture();

        const RenderImageInterfacePtr & image = texture->getImage();

        //ToDo: fix required setup alpha
        uint32_t hwChannels = image->getHWPixelFormat();

        if( hwChannels == 3 )
        {
            this->setAlpha( false );
        }
        else
        {
            this->setAlpha( true );
        }

        //ToDo: fix required setup pow2
        bool pow2 = texture->isPow2();

        this->setPow2( pow2 );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::correctUVTexture()
    {
        const mt::vec2f & size = this->getSize();

        uint32_t width = (uint32_t)(size.x + 0.5f);
        uint32_t height = (uint32_t)(size.y + 0.5f);

        mt::uv4f uvTextureImage;
        mt::uv4f uvTextureAlpha;

        const mt::uv4f & uvImage = this->getUVImage();
        const mt::uv4f & uvAlpha = this->getUVAlpha();

        if( Helper::isTexturePow2( width ) == false )
        {
            uint32_t width_pow2 = Helper::getTexturePow2( width );

            float scale = size.x / float( width_pow2 );

            for( uint32_t i = 0; i != 4; ++i )
            {
                uvTextureImage[i].x = uvImage[i].x * scale;
                uvTextureAlpha[i].x = uvAlpha[i].x * scale;
            }
        }
        else
        {
            for( uint32_t i = 0; i != 4; ++i )
            {
                uvTextureImage[i].x = uvImage[i].x;
                uvTextureAlpha[i].x = uvAlpha[i].x;
            }
        }

        if( Helper::isTexturePow2( height ) == false )
        {
            uint32_t height_pow2 = Helper::getTexturePow2( height );

            float scale = size.y / float( height_pow2 );

            for( uint32_t i = 0; i != 4; ++i )
            {
                uvTextureImage[i].y = uvImage[i].y * scale;
                uvTextureAlpha[i].y = uvAlpha[i].y * scale;
            }
        }
        else
        {
            for( uint32_t i = 0; i != 4; ++i )
            {
                uvTextureImage[i].y = uvImage[i].y;
                uvTextureAlpha[i].y = uvAlpha[i].y;
            }
        }

        this->setUVTextureImage( uvTextureImage );
        this->setUVTextureAlpha( uvTextureAlpha );
    }
    //////////////////////////////////////////////////////////////////////////
}
