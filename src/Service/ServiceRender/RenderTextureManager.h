#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "Core/ServiceBase.h"

#include "Core/ConstString.h"

#include "Factory/Factory.h"

#include "Config/Map.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
#	define MENGINE_TEXTURE_MANAGER_HASH_SIZE 4099
	//////////////////////////////////////////////////////////////////////////
    class RenderTextureManager
        : public ServiceBase<RenderTextureServiceInterface>
    {
    public:
        RenderTextureManager();
        ~RenderTextureManager() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
		RenderTextureInterfacePtr loadTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName, const ConstString& _codecName ) override;
		RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height ) override;

    public:        
		RenderTextureInterfacePtr createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
        RenderTextureInterfacePtr createDynamicTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;

	public:
        RenderTextureInterfacePtr getTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName ) const override;

    public:
        bool hasTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, RenderTextureInterfacePtr * _texture ) const override;

    public:
        void cacheFileTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName, const RenderTextureInterfacePtr & _texture ) override;

    public:
        bool saveImage( const RenderTextureInterfacePtr & _texture, const FileGroupInterfacePtr& _fileGroup, const ConstString & _codecName, const FilePath & _fileName ) override;

    public:
        void visitTexture( VisitorRenderTextureInterface * _visitor ) const override;
		size_t getImageMemoryUse( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) const override;

	protected:
		bool onRenderTextureDestroy_( RenderTextureInterface * _texture );		

	protected:
		void updateImageParams_( uint32_t & _width, uint32_t & _height, uint32_t & _channels, uint32_t & _depth, PixelFormat & _format ) const;

    protected:
		typedef std::pair<ConstString, FilePath> TMapRenderTextureKey;
		typedef Map<TMapRenderTextureKey, RenderTextureInterface *> TMapRenderTextureEntry;
		TMapRenderTextureEntry m_textures[MENGINE_TEXTURE_MANAGER_HASH_SIZE];

        FactoryPtr m_factoryRenderTexture;
        FactoryPtr m_factoryDecoderRenderImageProvider;

        uint32_t m_textureEnumerator;

		bool m_supportA8;
        bool m_supportL8;
		bool m_supportR8G8B8;
		bool m_supportNonPow2;

    private:
        TMapRenderTextureEntry & getHashEntry_(const ConstString & _fileName);
        const TMapRenderTextureEntry & getHashEntry_(const ConstString & _fileName) const;
    };
}