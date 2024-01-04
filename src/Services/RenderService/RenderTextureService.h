#pragma once

#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Hashtable2.h"
#include "Kernel/Pair.h"

namespace Mengine
{
    class RenderTextureService
        : public ServiceBase<RenderTextureServiceInterface>
    {
    public:
        RenderTextureService();
        ~RenderTextureService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        RenderTextureInterfacePtr createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc ) override;
        RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height, const DocumentInterfacePtr & _doc ) override;

    public:
        RenderTextureInterfacePtr loadTexture( const ContentInterfacePtr & _content, uint32_t _codecFlags, uint32_t _width, uint32_t _height, const DocumentInterfacePtr & _doc ) override;

    public:
        RenderTextureInterfacePtr getTexture( const ContentInterfacePtr & _content ) const override;

    public:
        bool hasTexture( const ContentInterfacePtr & _content, RenderTextureInterfacePtr * const _texture ) const override;

    public:
        RenderImageLoaderInterfacePtr createDecoderRenderImageLoader( const ContentInterfacePtr & _content, uint32_t _codecFlags, const DocumentInterfacePtr & _doc ) override;

    public:
        void cacheFileTexture( const ContentInterfacePtr & _content, const RenderTextureInterfacePtr & _texture ) override;

    public:
        bool saveImage( const RenderTextureInterfacePtr & _texture, const ContentInterfacePtr & _content ) override;

    public:
        void foreachTexture( const LambdaRenderTexture & _lambda ) const override;

    protected:
        bool onRenderTextureDestroy_( RenderTextureInterface * _texture );

    protected:
        typedef Hashtable2<FileGroupInterfacePtr, FilePath, RenderTextureInterface *> MapRenderTextureEntry;
        MapRenderTextureEntry m_textures;

        FactoryInterfacePtr m_factoryRenderTexture;
        FactoryInterfacePtr m_factoryDecoderRenderImageProvider;
        FactoryInterfacePtr m_factoryDecoderRenderImageLoader;
    };
}