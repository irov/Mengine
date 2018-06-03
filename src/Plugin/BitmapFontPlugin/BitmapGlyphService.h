#pragma once

#include "BitmapFontInterface.h"

#include "Kernel/Servant.h"

#include "Factory/Factory.h"

#include "Config/Map.h"

namespace Mengine
{
	class BitmapGlyphService
		: public ServantBase<BitmapGlyphServiceInterface>
	{
	public:
		BitmapGlyphService();
		~BitmapGlyphService() override;

	public:
		bool _initializeService() override;
		void _finalizeService() override;

	public:
		BitmapGlyphPtr getGlyph( const FileGroupInterfacePtr & _category, const FilePath & _path ) override;

	protected:
		FactoryPtr m_factoryTextGlyph;

		typedef Map<ConstString, BitmapGlyphPtr> TMapBitmapGlyph;
		TMapBitmapGlyph m_glyphs;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<BitmapGlyphService> BitmapGlyphServicePtr;
}