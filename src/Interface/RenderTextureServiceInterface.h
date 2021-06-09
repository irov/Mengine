#pragma once

#include "Interface/ServiceInterface.h"

#include "Interface/RenderTextureInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTextureServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "RenderTextureService" )

    public:
        virtual RenderTextureInterfacePtr createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentPtr & _doc ) = 0;
        virtual RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height, const DocumentPtr & _doc ) = 0;

    public:
        virtual RenderTextureInterfacePtr loadTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, uint32_t _codecFlags, const DocumentPtr & _doc ) = 0;        

    public:
        virtual RenderTextureInterfacePtr getTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) const = 0;
        virtual bool hasTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, RenderTextureInterfacePtr * const _texture ) const = 0;

    public:
        virtual RenderImageLoaderInterfacePtr createDecoderRenderImageLoader( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, uint32_t _codecFlags, const DocumentPtr & _doc ) = 0;

    public:
        virtual void cacheFileTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const RenderTextureInterfacePtr & _texture ) = 0;

    public:
        virtual bool saveImage( const RenderTextureInterfacePtr & _texture, const FileGroupInterfacePtr & _fileGroup, const ConstString & _codecType, const FilePath & _filePath ) = 0;

    public:
        typedef Lambda<void( const RenderTextureInterfacePtr & )> LambdaRenderTexture;
        virtual void visitTexture( const LambdaRenderTexture & _lambda ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RENDERTEXTURE_SERVICE()\
    ((Mengine::RenderTextureServiceInterface*)SERVICE_GET(Mengine::RenderTextureServiceInterface))
//////////////////////////////////////////////////////////////////////////
