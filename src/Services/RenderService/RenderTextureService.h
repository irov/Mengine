#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/ConstString.h"

#include "Kernel/Factory.h"

#include "Config/Pair.h"
#include "Config/Map.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_TEXTURE_MANAGER_HASH_SIZE
#define MENGINE_TEXTURE_MANAGER_HASH_SIZE 4099
#endif
//////////////////////////////////////////////////////////////////////////
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
        RenderTextureInterfacePtr loadTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName, const ConstString& _codecName, const Char * _doc ) override;
        RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height, const Char * _doc ) override;

    public:
        RenderTextureInterfacePtr createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const Char * _doc ) override;
        RenderTextureInterfacePtr createDynamicTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const Char * _doc ) override;

    public:
        RenderTextureInterfacePtr getTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName ) const override;

    public:
        bool hasTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, RenderTextureInterfacePtr * _texture ) const override;

    public:
        void cacheFileTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName, const RenderTextureInterfacePtr & _texture ) override;

    public:
        bool saveImage( const RenderTextureInterfacePtr & _texture, const FileGroupInterfacePtr& _fileGroup, const ConstString & _codecName, const FilePath & _fileName ) override;

    public:
        void visitTexture( const LambdaRenderTexture & _lambda ) const override;
        size_t getImageMemoryUse( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format ) const override;

    protected:
        bool onRenderTextureDestroy_( RenderTextureInterface * _texture );

    protected:
        void updateImageParams_( uint32_t & _width, uint32_t & _height, uint32_t & _channels, uint32_t & _depth, EPixelFormat & _format ) const;

    protected:
        typedef Pair<ConstString, FilePath> MapRenderTextureKey;
        typedef Map<MapRenderTextureKey, RenderTextureInterface *> MapRenderTextureEntry;
        MapRenderTextureEntry m_textures[MENGINE_TEXTURE_MANAGER_HASH_SIZE];

        FactoryPtr m_factoryRenderTexture;
        FactoryPtr m_factoryDecoderRenderImageProvider;

        bool m_supportA8;
        bool m_supportL8;
        bool m_supportR8G8B8;
        bool m_supportNonPow2;

    private:
        MapRenderTextureEntry & getHashEntry_( const ConstString & _fileName );
        const MapRenderTextureEntry & getHashEntry_( const ConstString & _fileName ) const;
    };
}