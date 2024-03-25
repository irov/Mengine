#include "ResourceImageEmpty.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/RenderServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstString.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/PixelFormatHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageEmpty::ResourceImageEmpty()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageEmpty::~ResourceImageEmpty()
    {
        MENGINE_ASSERTION_FATAL( this->getTexture() == nullptr, "texture is not nullptr" );
        MENGINE_ASSERTION_FATAL( this->getTextureAlpha() == nullptr, "texture alpha is not nullptr" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageEmpty::_compile()
    {
        RenderTextureInterfacePtr texture = RENDER_SERVICE()
            ->getNullTexture();

        MENGINE_ASSERTION_MEMORY_PANIC( texture, "name '%s' invalid get null texture"
            , this->getName().c_str()
        );

        this->setTexture( texture );

        this->prepareImageFrame_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageEmpty::_release()
    {
        ResourceImage::_release();

        this->setTexture( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageEmpty::prepareImageFrame_()
    {
        const RenderTextureInterfacePtr & texture = this->getTexture();

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
}
