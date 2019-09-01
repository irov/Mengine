#pragma once

#include "Interface/ServiceInterface.h"

#include "Interface/RenderTextureInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTextureServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "RenderTextureService" )

    public:
        virtual RenderTextureInterfacePtr loadTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codec, const Char * _doc ) = 0;
        virtual RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height, const Char * _doc ) = 0;

    public:
        virtual RenderTextureInterfacePtr createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const Char * _doc ) = 0;
        virtual RenderTextureInterfacePtr createDynamicTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const Char * _doc ) = 0;

    public:
        virtual RenderTextureInterfacePtr getTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) const = 0;

        virtual bool hasTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, RenderTextureInterfacePtr * _texture ) const = 0;

    public:
        virtual size_t getImageMemoryUse( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format ) const = 0;

    public:
        virtual void cacheFileTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const RenderTextureInterfacePtr & _texture ) = 0;

    public:
        virtual bool saveImage( const RenderTextureInterfacePtr & _texture, const FileGroupInterfacePtr & _fileGroup, const ConstString & _codecName, const FilePath & _filePath ) = 0;

    public:
        typedef Lambda<void( const RenderTextureInterfacePtr & )> LambdaRenderTexture;
        virtual void visitTexture( const LambdaRenderTexture & _lambda ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RENDERTEXTURE_SERVICE()\
    ((Mengine::RenderTextureServiceInterface*)SERVICE_GET(Mengine::RenderTextureServiceInterface))
//////////////////////////////////////////////////////////////////////////
