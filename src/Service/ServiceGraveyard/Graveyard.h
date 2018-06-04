#pragma once

#include "Interface/GraveyardInterface.h"

#include "Core/ServiceBase.h"

namespace Mengine
{
    typedef IntrusivePtr<class RenderImageInterface> RenderImageInterfacePtr;

	struct RenderTextureGraveEntry
	{
        FileGroupInterfacePtr category;
		FilePath filePath;

		RenderImageInterfacePtr image;
		uint32_t width;
		uint32_t height;

        float timing;
	};

	class Graveyard
		: public ServiceBase<GraveyardInterface>
	{
	public:
		Graveyard();
		~Graveyard() override;

	public:
		bool _initializeService() override;
		void _finalizeService() override;

	public:
		void tick( float _time, float _timing ) override;

	public:
		void clearTextures() override;

	public:
		bool buryTexture( RenderTextureInterface * _texture ) override;
		RenderTextureInterfacePtr resurrectTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath ) override;

	protected:
		void updateTexture_( RenderTextureGraveEntry * _entry, float _timing );

	protected:
		uint32_t m_count;
		float m_graveyardTime;

		typedef Vector<RenderTextureGraveEntry> TVectorTextureGrave;
		TVectorTextureGrave m_textures;
	};
}