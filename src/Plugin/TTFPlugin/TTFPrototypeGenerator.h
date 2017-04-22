#	pragma once

#	include "Interface/PrototypeManagerInterface.h"

#	include "Core/ServantBase.h"

#	include "Factory/Factory.h"

#	include "ft2build.h"
#	include "freetype/freetype.h"
#	include "freetype/ftglyph.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class TTFPrototypeGenerator
		: public ServantBase<PrototypeGeneratorInterface>
	{
	public:
		TTFPrototypeGenerator();
		~TTFPrototypeGenerator();

	public:
		void setFTLibrary( FT_Library _library );

	public:
		bool initialize( const ConstString & _category, const ConstString & _prototype ) override;

	public:
		PointerFactorable generate() override;

	public:
		uint32_t count() const override;

	protected:
		FT_Library m_library;

		FactoryPtr m_factoryFont;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TTFPrototypeGenerator> TTFPrototypeGeneratorPtr;
	//////////////////////////////////////////////////////////////////////////
}