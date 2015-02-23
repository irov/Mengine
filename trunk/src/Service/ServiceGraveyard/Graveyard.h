#	pragma once

#	include "Interface/GraveyardInterface.h"

#	include "stdex/binary_vector.h"

namespace Menge
{
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
		void buryTexture( const FilePath & _path, RenderTextureInterface * _texture ) override;
		RenderTextureInterfacePtr resurrectTexture( const FilePath & _path ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		float m_graveyardTime;

		struct RenderTextureGraveDesc
		{
			RenderImageInterfacePtr image;
			uint32_t mipmaps;
			uint32_t width;
			uint32_t height;
			uint32_t channels;
			float timing;
		};

		typedef stdex::binary_vector<FilePath, RenderTextureGraveDesc> TMapTextureGrave;
		TMapTextureGrave m_textures;
	};
}