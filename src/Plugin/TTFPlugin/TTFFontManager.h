#	pragma once

#	include "TTFServiceInterface.h"

#	include "stdex/stl_map.h"

#	include "ft2build.h"
#	include "freetype/freetype.h"
#	include "freetype/ftglyph.h"

namespace Menge
{
	typedef stdex::intrusive_ptr<class TTFFont> TTFFontPtr;

	class TTFFontManager
		: public ServiceBase<TTFFontManagerInterface>
	{
	public:
		TTFFontManager();
		~TTFFontManager();

	protected:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool createFont( const ConstString & _name, const ConstString& _pakName, const FilePath& _fileName ) override;
		TTFFontInterfacePtr getFont( const ConstString & _name ) const override;

	protected:
		FT_Library m_library;

		typedef stdex::map<ConstString, TTFFontInterfacePtr> TMapFonts;
		TMapFonts m_fonts;
	};
}