#include "ResourceImageDefault.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstString.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefault::ResourceImageDefault()
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        : m_forcePremultiply( false )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDefault::~ResourceImageDefault()
    {
        MENGINE_ASSERTION_FATAL( this->getTexture( 0 ) == nullptr, "texture is not nullptr" );
        MENGINE_ASSERTION_FATAL( this->getTexture( 1 ) == nullptr, "texture alpha is not nullptr" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDefault::_compile()
    {
        if( ResourceImage::_compile() == false )
        {
            LOGGER_ERROR( "resource image '%s' invalid compile base ResourceImage class"
                , this->getName().c_str()
            );

            return false;
        }

        uint32_t decoder_options = DF_IMAGE_NONE;

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        bool Engine_ForcePremultiplyAlpha = CONFIG_VALUE_BOOLEAN( "Engine", "ForcePremultiplyAlpha", false );

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

        bool trimAtlas = this->isTrimAtlas();
        const mt::vec2f & maxSize = this->getMaxSize();
        uint32_t maxSize_width = (uint32_t)(maxSize.x + 0.5f);
        uint32_t maxSize_height = (uint32_t)(maxSize.y + 0.5f);

        uint32_t width = trimAtlas == true ? maxSize_width : MENGINE_UINT32_C(~0);
        uint32_t height = trimAtlas == true ? maxSize_height : MENGINE_UINT32_C(~0);

        const ContentInterfacePtr & content = this->getContent();

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->loadTexture( content, decoder_options, width, height, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( texture, "name '%s' category '%s' group '%s' can't load image file '%s'"
            , this->getName().c_str()
            , this->getContent()->getFileGroup()->getName().c_str()
            , this->getGroupName().c_str()
            , Helper::getContentFullPath( this->getContent() ).c_str()
        );

        this->setTexture( 0, texture );

        this->prepareImageFrame_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageDefault::_release()
    {
        this->setTexture( 0, nullptr );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
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
        const RenderTextureInterfacePtr & texture = this->getTexture( 0 );

        const RenderImageInterfacePtr & image = texture->getImage();

        //ToDo: fix required setup alpha
        EPixelFormat hwPixelFormat = image->getHWPixelFormat();

        uint32_t hwPixelChannels = Helper::getPixelFormatChannels( hwPixelFormat );

        if( hwPixelChannels == 3 )
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

        bool trimAtlas = this->isTrimAtlas();        

        if( trimAtlas == false )
        {
            mt::uv4f uvTexture[MENGINE_RESOURCEIMAGE_MAX_TEXTURE];

            if( Helper::isTexturePow2( width ) == false )
            {
                uint32_t width_pow2 = Helper::getTexturePow2( width );

                float scale = size.x / float( width_pow2 );

                for( size_t index = 0; index != MENGINE_RESOURCEIMAGE_MAX_TEXTURE; ++index )
                {
                    const mt::uv4f & uv = this->getUV( index );

                    for( uint32_t i = 0; i != 4; ++i )
                    {
                        uvTexture[index][i].x = uv[i].x * scale;
                    }
                }
            }
            else
            {
                for( size_t index = 0; index != MENGINE_RESOURCEIMAGE_MAX_TEXTURE; ++index )
                {
                    const mt::uv4f & uv = this->getUV( index );

                    for( uint32_t i = 0; i != 4; ++i )
                    {
                        uvTexture[index][i].x = uv[i].x;
                    }
                }
            }

            if( Helper::isTexturePow2( height ) == false )
            {
                uint32_t height_pow2 = Helper::getTexturePow2( height );

                float scale = size.y / float( height_pow2 );

                for( size_t index = 0; index != MENGINE_RESOURCEIMAGE_MAX_TEXTURE; ++index )
                {
                    const mt::uv4f & uv = this->getUV( index );

                    for( uint32_t i = 0; i != 4; ++i )
                    {
                        uvTexture[index][i].y = uv[i].y * scale;
                    }
                }
            }
            else
            {
                for( size_t index = 0; index != MENGINE_RESOURCEIMAGE_MAX_TEXTURE; ++index )
                {
                    const mt::uv4f & uv = this->getUV( index );

                    for( uint32_t i = 0; i != 4; ++i )
                    {
                        uvTexture[index][i].y = uv[i].y;
                    }
                }
            }

            for( size_t index = 0; index != MENGINE_RESOURCEIMAGE_MAX_TEXTURE; ++index )
            {
                const mt::uv4f & uv = uvTexture[index];

                this->setUVTexture( index, uv );
            }
        }
        else
        {
            for( size_t index = 0; index != MENGINE_RESOURCEIMAGE_MAX_TEXTURE; ++index )
            {
                const mt::uv4f & uv = this->getUV( index );

                this->setUVTexture( index, uv );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
