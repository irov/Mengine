#	pragma once

#	include "TTFAtlasServiceInterface.h"

#	include "stdex/stl_vector.h"

namespace Menge
{
	struct TTFAtlas
	{
		uint32_t dimension;

		typedef stdex::vector<uint32_t> TVectorFreeIndices;
		TVectorFreeIndices indices;

		RenderTextureInterfacePtr texture;
	};

	class TTFAtlasService
		: public ServiceBase<TTFAtlasServiceInterface>
	{
	public:
		TTFAtlasService();
		~TTFAtlasService();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		RenderTextureInterfacePtr makeTextureGlyph( uint32_t _width, uint32_t _height, const void * _buffer, uint32_t _pitch, mt::uv4f & _uv ) override;
		
	protected:
		TTFAtlas * getAtlas_( uint32_t _index );

	public:
		typedef stdex::vector<TTFAtlas> TVectorAtlases;
		typedef stdex::vector<TVectorAtlases> TVectorAtlasess;
		TVectorAtlasess m_atlasess;

		uint32_t m_minAtlasPow;
		uint32_t m_maxAtlasPow;
		uint32_t m_maxAtlasWidth;
	};
}