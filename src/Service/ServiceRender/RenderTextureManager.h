#   pragma once

#   include "Interface/RenderSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#   include "RenderTexture.h"

#	include "Core/ConstString.h"

#   include "stdex/binary_vector.h"

#   include "Factory/FactoryPool.h"

#	include <list>

namespace Menge
{
	class RenderTexturePrefetcherDesc
		: public RenderTextureDecoderReceiverInterface
	{
	public:
		RenderTexturePrefetcherDesc()
		{
		}

	public:
		const ImageDecoderInterfacePtr & getDecoder() const
		{
			return m_decoder;
		}

	public:
		bool addListener( RenderTextureDecoderListenerInterface * _listener )
		{
			if( m_decoder == nullptr )
			{
				m_listeners.push_back( _listener );

				return true;
			}

			_listener->onTextureDecoderReady( m_decoder );

			return false;
		}

	public:
		void unfetch()
		{
			m_decoder = nullptr;

			for( TVectorListeners::iterator
				it = m_listeners.begin(),
				it_end = m_listeners.end();
			it != it_end;
			++it )
			{
				RenderTextureDecoderListenerInterface * listener = *it;

				listener->onTextureDecoderCancel();
			}

			m_listeners.clear();
		}

	protected:
		void onReceiverTextureDecoder( const ImageDecoderInterfacePtr & _decoder ) override
		{
			m_decoder = _decoder;

			for( TVectorListeners::iterator
				it = m_listeners.begin(),
				it_end = m_listeners.end();
			it != it_end;
			++it )
			{
				RenderTextureDecoderListenerInterface * listener = *it;

				listener->onTextureDecoderReady( m_decoder );
			}

			m_listeners.clear();
		}

	protected:
		ImageDecoderInterfacePtr m_decoder;

		typedef std::list<RenderTextureDecoderListenerInterface *> TVectorListeners;
		TVectorListeners m_listeners;
	};


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
		RenderTextureDecoderReceiverInterface * prefetchTextureDecoder( const FilePath& _filename, RenderTextureDecoderListenerInterface * _listener ) override;
		void unfetchTextureDecoder( const FilePath& _filename ) override;

	protected:
		bool hasTextureDecoder( const FilePath& _filename, ImageDecoderInterfacePtr & _decoder ) const;

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

		typedef stdex::binary_vector<FilePath, RenderTexturePrefetcherDesc *> TMapPrefetchers;
		TMapPrefetchers m_prefetchers;

        typedef FactoryPool<RenderTexture, 128> TFactoryRenderTexture;
        TFactoryRenderTexture m_factoryRenderTexture;

        size_t m_textureEnumerator;

        RenderTextureDebugInfo m_debugInfo;
		
		bool m_supportA8;
		bool m_supportR8G8B8;
    };
}