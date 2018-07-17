#pragma once

#include "TTFServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Vector.h"

namespace Mengine
{
	struct TTFAtlas
	{
		uint32_t dimension;
        uint32_t channel;

		typedef Vector<uint32_t> TVectorFreeIndices;
		TVectorFreeIndices indices;
        
		RenderTextureInterfacePtr texture;
	};

	class TTFAtlasService
		: public ServiceBase<TTFAtlasServiceInterface>
	{
	public:
		TTFAtlasService();
		~TTFAtlasService() override;

	public:
		bool _initializeService() override;
		void _finalizeService() override;

	public:
		RenderTextureInterfacePtr makeTextureGlyph( uint32_t _width, uint32_t _height, uint32_t _channel, TextureGlyphProviderInterface * _provider, mt::uv4f & _uv ) override;
		
	protected:
		TTFAtlas * getAtlas_( uint32_t _index, uint32_t _channel );

	public:
		typedef Vector<TTFAtlas> TVectorAtlases;
		typedef Vector<TVectorAtlases> TVectorAtlasess;
		TVectorAtlasess m_atlasess[3];
        
		uint32_t m_minAtlasPow;
		uint32_t m_maxAtlasPow;
		uint32_t m_maxAtlasWidth;
	};
}