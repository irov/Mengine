#   pragma once

#   include "Interface/RenderSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#   include "RenderTexture.h"

#	include "Core/ConstString.h"

#   include "stdex/binary_vector.h"

#   include "Factory/FactoryPool.h"
#	include "Factory/FactoryDefault.h"

#	include <list>

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
        RenderTextureInterfacePtr loadTexture( const ConstString& _pakName, const FilePath& _filename, const ConstString& _codec ) override;

	protected:
		ImageDecoderInterfacePtr createImageDecoder_( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec );
		RenderTextureInterfacePtr createTextureFromDecoder_( const ImageDecoderInterfacePtr & _decoder );

    public:
        RenderTextureInterfacePtr createTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
        RenderTextureInterfacePtr createDynamicTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
        RenderTextureInterfacePtr createRenderTargetTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;

        RenderTextureInterfacePtr getTexture( const FilePath & _filename ) const override;

    public:
        bool hasTexture( const FilePath & _filename, RenderTextureInterfacePtr * _texture ) const override;

    public:
        void imageQuality( const RenderTextureInterfacePtr & _texture, unsigned char * _textureBuffer, size_t _texturePitch ) override;

        void cacheFileTexture( const FilePath& _filename, const RenderTextureInterfacePtr & _texture ) override;

    public:
        bool saveImage( const RenderTextureInterfacePtr & _texture, const ConstString& _fileSystemName, const ConstString & _codecName, const FilePath & _filename ) override;

    public:
        void visitTexture( VisitorRenderTextureInterface * _visitor ) const override;

    public:
        const RenderTextureDebugInfo & getDebugInfo() override;

    protected:
        void onRenderTextureRelease( const RenderTextureInterface * _texture ) override;

    protected:
        bool loadTextureRectImageData( const RenderTextureInterfacePtr & _texture, const Rect & _rect, const ImageDecoderInterfacePtr & _imageDecoder );

	protected:
		void updateImageParams_( size_t & _width, size_t & _height, size_t & _channels, PixelFormat & _format ) const;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef stdex::binary_vector<FilePath, RenderTextureInterface *> TMapTextures;
        TMapTextures m_textures;

        typedef FactoryPool<RenderTexture, 128> TFactoryRenderTexture;
        TFactoryRenderTexture m_factoryRenderTexture;

        size_t m_textureEnumerator;

        RenderTextureDebugInfo m_debugInfo;
		
		bool m_supportA8;
		bool m_supportR8G8B8;
    };
}