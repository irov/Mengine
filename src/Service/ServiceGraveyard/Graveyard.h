#	pragma once

#	include "Interface/GraveyardInterface.h"

#   include "Core/ServiceBase.h"

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
		: public ServiceBase<GraveyardInterface>
	{
	public:
		Graveyard();
		~Graveyard();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		void tick( float _time, float _timing ) override;

	public:
		void clearTextures() override;

	public:
		bool buryTexture( RenderTextureInterface * _texture ) override;
		RenderTextureInterfacePtr resurrectTexture( const ConstString& _pakName, const FilePath & _filePath ) override;

	protected:
		void updateTexture_( RenderTextureGraveEntry * _entry, float _timing );

	protected:
		uint32_t m_count;
		float m_graveyardTime;

		typedef stdex::vector<RenderTextureGraveEntry> TVectorTextureGrave;
		TVectorTextureGrave m_textures;
	};
}