#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "stdex/stl_vector.h"

#	define MENGINE_TTF_MIN_ATLAS_POW 4
#	define MENGINE_TTF_MAX_ATLAS_POW 8
#	define MENGINE_TTF_MIX_ATLAS_POW (MENGINE_TTF_MAX_ATLAS_POW - MENGINE_TTF_MIN_ATLAS_POW)

namespace Menge
{
	class TTFAtlasManager
		: public FactorablePtr
	{
	public:
		TTFAtlasManager();
		~TTFAtlasManager();

	public:
		RenderTextureInterfacePtr makeTextureGlyph( uint32_t _height, uint32_t _width, const void * _buffer, uint32_t _pitch );

	public:
		struct Atlas
		{
			typedef stdex::vector<uint32_t> TVectorFreeIndices;
			TVectorFreeIndices indices;

			RenderTextureInterfacePtr texture;
		};

		typedef stdex::vector<Atlas> TVectorAtlases;
		TVectorAtlases m_atlasess[MENGINE_TTF_MIX_ATLAS_POW];
	};
}