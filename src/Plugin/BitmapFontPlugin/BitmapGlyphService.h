#pragma once

#include "Interface/TextInterface.h"

#include "BitmapFontInterface.h"

#include "Kernel/Servant.h"

#include "Factory/Factory.h"

#include "stdex/stl_map.h"

namespace Mengine
{
	class BitmapGlyphService
		: public ServantBase<BitmapGlyphServiceInterface>
	{
	public:
		BitmapGlyphService();
		~BitmapGlyphService();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		BitmapGlyphPtr getGlyph( const ConstString & _category, const FilePath & _path ) override;

	protected:
		FactoryPtr m_factoryTextGlyph;

		typedef stdex::map<ConstString, BitmapGlyphPtr> TMapBitmapGlyph;
		TMapBitmapGlyph m_glyphs;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<BitmapGlyphService> BitmapGlyphServicePtr;
}