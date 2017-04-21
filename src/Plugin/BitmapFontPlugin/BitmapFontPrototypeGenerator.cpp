#	include "BitmapFontPrototypeGenerator.h"

#	include "Factory/FactoryPool.h"

#	include "BitmapFont.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool BitmapFontPrototypeGenerator::initialize( const ConstString & _category, const ConstString & _prototype )
	{
		(void)_category;
		(void)_prototype;

		m_factoryBitmapFont = new FactoryPool<BitmapFont, 8>( m_serviceProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PointerFactorable BitmapFontPrototypeGenerator::generate()
	{
		BitmapFont * font = m_factoryBitmapFont->createObject();

		font->setServiceProvider( m_serviceProvider );

		return font;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t BitmapFontPrototypeGenerator::count() const
	{
		uint32_t count = m_factoryBitmapFont->countObject();

		return count;
	}
}