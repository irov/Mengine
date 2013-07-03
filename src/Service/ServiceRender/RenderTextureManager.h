#   pragma once

#   include "Interface/RenderSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#   include "RenderTexture.h"

#	include "Core/ConstString.h"
#	include "Core/BinaryVector.h"

#   include "Factory/FactoryPool.h"

namespace Menge
{
    class RenderTextureManager
        : public RenderTextureServiceInterface
        , public RenderTextureInterfaceListener
    {
    public:
        RenderTextureManager();
        ~RenderTextureManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _provider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        RenderTextureInterfacePtr loadTexture( const ConstString& _pakName, const FilePath& _filename, const ConstString& _codec, size_t _width, size_t _height ) override;

    public:
        RenderTextureInterfacePtr createTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format, size_t _textureWidth, size_t _textureHeight ) override;
        RenderTextureInterfacePtr createDynamicTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
        RenderTextureInterfacePtr createRenderTargetTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;

        RenderTextureInterfacePtr getTexture( const FilePath& _filename ) override;

    public:
        bool hasTexture( const FilePath & _filename, RenderTextureInterfacePtr * _texture ) const override;

    public:
        void imageQuality( const RenderTextureInterfacePtr & _texture, unsigned char * _textureBuffer, size_t _texturePitch ) override;

        void cacheFileTexture( const FilePath& _filename, const RenderTextureInterfacePtr & _texture ) override;

    public:
        bool saveImage( const RenderTextureInterfacePtr & _texture, const ConstString& _fileSystemName, const ConstString & _codecName, const FilePath & _filename ) override;

    public:
        const RenderTextureDebugInfo & getDebugInfo() override;

    protected:
        void onRenderTextureRelease( const RenderTextureInterface * _texture ) override;

    protected:
        bool loadTextureRectImageData( const RenderTextureInterfacePtr & _texture, const Rect & _rect, const ImageDecoderInterfacePtr & _imageDecoder );
        bool createNullTexture_();

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef BinaryVector<FilePath, RenderTextureInterface *> TMapTextures;
        TMapTextures m_textures;

        typedef FactoryPool<RenderTexture, 128> TFactoryRenderTexture;
        TFactoryRenderTexture m_factoryRenderTexture;

        size_t m_textureEnumerator;

        RenderTextureDebugInfo m_debugInfo;	    // debug info
    };
}