#pragma once

#include "TTFServiceInterface.h"

#include "Core/ServiceBase.h"

#include "stdex/stl_vector.h"

namespace Mengine
{
	struct TTFAtlas
	{
		uint32_t dimension;
        uint32_t channel;

		typedef stdex::vector<uint32_t> TVectorFreeIndices;
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
		bool _initialize() override;
		void _finalize() override;

	public:
		RenderTextureInterfacePtr makeTextureGlyph( uint32_t _width, uint32_t _height, uint32_t _channel, TextureGlyphProviderInterface * _provider, mt::uv4f & _uv ) override;
		
	protected:
		TTFAtlas * getAtlas_( uint32_t _index, uint32_t _channel );

	public:
		typedef stdex::vector<TTFAtlas> TVectorAtlases;
		typedef stdex::vector<TVectorAtlases> TVectorAtlasess;
		TVectorAtlasess m_atlasess[2];
        
		uint32_t m_minAtlasPow;
		uint32_t m_maxAtlasPow;
		uint32_t m_maxAtlasWidth;
	};
}