#	pragma once

#	include "Interface/TextInterface.h"

#	include "Kernel/Servant.h"

#	include "Core/ServiceBase.h"

#	include "BitmapGlyph.h"

namespace Menge
{
	class BitmapGlyphServiceInterface
		: public ServiceBase<ServiceInterface>
	{
		SERVICE_DECLARE( "BitmapGlyphService" )

	public:
		virtual BitmapGlyphPtr getGlyph( const ConstString & _category, const FilePath & _path ) = 0;
	};
}

#   define BITMAPGLYPH_SERVICE()\
    ((Menge::BitmapGlyphServiceInterface *)SERVICE_GET(Menge::BitmapGlyphServiceInterface))
