#include "BitmapFontPrototypeGenerator.h"

#include "Factory/FactoryPool.h"

#include "BitmapFont.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	bool BitmapFontPrototypeGenerator::initialize( const ConstString & _category, const ConstString & _prototype )
	{
		(void)_category;
		(void)_prototype;

		m_factoryFont = new FactoryPool<BitmapFont, 8>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PointerFactorable BitmapFontPrototypeGenerator::generate()
	{
		BitmapFont * font = m_factoryFont->createObject();
        
		return font;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t BitmapFontPrototypeGenerator::count() const
	{
		uint32_t count = m_factoryFont->countObject();

		return count;
	}
}