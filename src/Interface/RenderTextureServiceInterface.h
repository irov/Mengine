#pragma once

#include "Interface/ServiceInterface.h"

#include "Interface/RenderTextureInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/ContentInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class RenderTextureServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "RenderTextureService" )

    public:
        virtual RenderTextureInterfacePtr createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc ) = 0;
        virtual RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual RenderTextureInterfacePtr loadTexture( const ContentInterfacePtr & _content, uint32_t _codecFlags, uint32_t _width, uint32_t _height, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual RenderTextureInterfacePtr getTexture( const ContentInterfacePtr & _content ) const = 0;
        virtual bool hasTexture( const ContentInterfacePtr & _content, RenderTextureInterfacePtr * const _texture ) const = 0;

    public:
        virtual RenderImageLoaderInterfacePtr createDecoderRenderImageLoader( const ContentInterfacePtr & _content, uint32_t _codecFlags, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual void cacheFileTexture( const ContentInterfacePtr & _content, const RenderTextureInterfacePtr & _texture ) = 0;

    public:
        virtual bool saveImage( const RenderTextureInterfacePtr & _texture, const ContentInterfacePtr & _content ) = 0;

    public:
        typedef Lambda<void( const RenderTextureInterfacePtr & )> LambdaRenderTexture;
        virtual void foreachTexture( const LambdaRenderTexture & _lambda ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RENDERTEXTURE_SERVICE()\
    ((Mengine::RenderTextureServiceInterface *)SERVICE_GET(Mengine::RenderTextureServiceInterface))
//////////////////////////////////////////////////////////////////////////
