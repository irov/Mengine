#include "ResourceImageEmpty.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/RenderServiceInterface.h"

#include "Kernel/Logger.h"
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
        MENGINE_ASSERTION_FATAL( this->getTexture( 0 ) == nullptr, "texture is not nullptr" );
        MENGINE_ASSERTION_FATAL( this->getTexture( 1 ) == nullptr, "texture alpha is not nullptr" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageEmpty::_compile()
    {
        RenderTextureInterfacePtr texture = RENDER_SERVICE()
            ->getNullTexture();

        MENGINE_ASSERTION_MEMORY_PANIC( texture, "name '%s' invalid get null texture"
            , this->getName().c_str()
        );

        this->setTexture( 0, texture );

        this->prepareImageFrame_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageEmpty::_release()
    {
        ResourceImage::_release();

        this->setTexture( 0, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageEmpty::prepareImageFrame_()
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
}
