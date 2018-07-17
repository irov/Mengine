#pragma once

#include "Interface/TextInterface.h"

#include "Kernel/Servant.h"

#include "Kernel/ServiceBase.h"

#include "BitmapGlyph.h"

namespace Mengine
{
	class BitmapGlyphServiceInterface
		: public ServiceBase<ServiceInterface>
	{
		SERVICE_DECLARE( "BitmapGlyphService" )

	public:
		virtual BitmapGlyphPtr getGlyph( const FileGroupInterfacePtr & _category, const FilePath & _path ) = 0;
	};
}

#   define BITMAPGLYPH_SERVICE()\
    ((Mengine::BitmapGlyphServiceInterface *)SERVICE_GET(Mengine::BitmapGlyphServiceInterface))
