#pragma once

#include "Kernel/PluginBase.h"

#include "TTFServiceInterface.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class TTFPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "TTF" )

	public:
		TTFPlugin();
        ~TTFPlugin() override;

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		FT_Library m_ftlibrary;
	};
}