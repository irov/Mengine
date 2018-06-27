#pragma once

#include "Interface/ServiceInterface.h"

#include "Interface/RenderTextureInterface.h"
#include "Interface/FileGroupInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class VisitorRenderTextureInterface
    {
    public:
        virtual void visitRenderTexture( const RenderTextureInterfacePtr & _texture ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class RenderTextureServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "RenderTextureManager" )

    public:
        virtual RenderTextureInterfacePtr loadTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName, const ConstString& _codec ) = 0;
        virtual RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height ) = 0;

    public:
        virtual RenderTextureInterfacePtr createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) = 0;
        virtual RenderTextureInterfacePtr createDynamicTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) = 0;

    public:
        virtual RenderTextureInterfacePtr getTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName ) const = 0;

        virtual bool hasTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, RenderTextureInterfacePtr * _texture ) const = 0;

    public:
        virtual size_t getImageMemoryUse( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) const = 0;

    public:
        virtual void cacheFileTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName, const RenderTextureInterfacePtr & _texture ) = 0;

    public:
        virtual bool saveImage( const RenderTextureInterfacePtr & _texture, const FileGroupInterfacePtr& _fileGroup, const ConstString & _codecName, const FilePath & _fileName ) = 0;

    public:
        virtual void visitTexture( VisitorRenderTextureInterface * _visitor ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define RENDERTEXTURE_SERVICE()\
    ((Mengine::RenderTextureServiceInterface*)SERVICE_GET(Mengine::RenderTextureServiceInterface))
}