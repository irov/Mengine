#	pragma once

#	include "Interface/GraveyardInterface.h"

#	include "Core/IntrusiveDuplexTree.h"

namespace Menge
{
	struct RenderTextureGraveEntry
	{
		ConstString category;
		FilePath filePath;

		RenderImageInterfacePtr image;
		uint32_t mipmaps;
		uint32_t width;
		uint32_t height;
		uint32_t channels;
		float timing;
	};

	class Graveyard
		: public GraveyardInterface
	{
	public:
		Graveyard();
		~Graveyard();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		void update( float _timing ) override;

	public:
		void clearTextures() override;

	public:
		bool buryTexture( RenderTextureInterface * _texture ) override;
		RenderTextureInterfacePtr resurrectTexture( const ConstString& _pakName, const FilePath & _filePath ) override;

	protected:
		void updateTexture_( RenderTextureGraveEntry * _entry, float _timing );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		uint32_t m_count;
		float m_graveyardTime;

		typedef stdex::vector<RenderTextureGraveEntry> TVectorTextureGrave;
		TVectorTextureGrave m_textures;
	};
}