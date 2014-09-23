#   pragma once

#   include "Interface/RenderSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#   include "RenderTexture.h"

#	include "Core/ConstString.h"

#   include "stdex/binary_vector.h"

#   include "Factory/FactoryStore.h"
#	include "Factory/FactoryDefault.h"

namespace Menge
{
    class RenderTextureManager
        : public RenderTextureServiceInterface
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
        RenderTextureInterfacePtr loadTexture( const ConstString& _pakName, const FilePath& _fileName, const ConstString& _codec ) override;
		RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height, uint32_t _channels ) override;

	protected:
		ImageDecoderInterfacePtr createImageDecoder_( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codec );
		RenderTextureInterfacePtr createTextureFromDecoder_( const ImageDecoderInterfacePtr & _decoder );

    public:
        RenderTextureInterfacePtr createTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
        RenderTextureInterfacePtr createDynamicTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
        RenderTextureInterfacePtr createRenderTargetTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;

        RenderTextureInterfacePtr getTexture( const FilePath & _fileName ) const override;

    public:
        bool hasTexture( const FilePath & _fileName, RenderTextureInterfacePtr * _texture ) const override;

    public:
        void imageQuality( const RenderTextureInterfacePtr & _texture, void * _textureBuffer, size_t _texturePitch ) override;

        void cacheFileTexture( const FilePath& _fileName, const RenderTextureInterfacePtr & _texture ) override;

    public:
        bool saveImage( const RenderTextureInterfacePtr & _texture, const ConstString& _fileGroupName, const ConstString & _codecName, const FilePath & _fileName ) override;

    public:
        void visitTexture( VisitorRenderTextureInterface * _visitor ) const override;
		size_t getImageMemoryUse( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) const override;

    public:
        const RenderTextureDebugInfo & getDebugInfo() override;

	protected:
        bool onRenderTextureDestroy_( RenderTextureInterface * _texture );				
		void releaseRenderTexture_( RenderTextureInterface * _texture );

    protected:
        bool loadTextureRectImageData( const RenderTextureInterfacePtr & _texture, const Rect & _rect, const ImageDecoderInterfacePtr & _imageDecoder );

	protected:
		void updateImageParams_( uint32_t & _width, uint32_t & _height, uint32_t & _channels, uint32_t & _depth, PixelFormat & _format ) const;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef stdex::binary_vector<FilePath, RenderTextureInterface *> TMapTextures;
        TMapTextures m_textures;

        typedef FactoryPoolStore<RenderTexture, 128> TFactoryRenderTexture;
        TFactoryRenderTexture m_factoryRenderTexture;

        uint32_t m_textureEnumerator;

        RenderTextureDebugInfo m_debugInfo;

		uint32_t m_limitTextureWidth;
		uint32_t m_limitTextureHeight;
		
		bool m_supportA8;
		bool m_supportR8G8B8;
    };
}