#include "BitmapFontPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"

#include "BitmapFont.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BitmapFontPrototypeGenerator::BitmapFontPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    BitmapFontPrototypeGenerator::~BitmapFontPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryPtr BitmapFontPrototypeGenerator::_initializeFactory()
    {
        FactoryPtr factory = new FactoryPool<BitmapFont, 8>();

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer BitmapFontPrototypeGenerator::generate()
    {
        const FactoryPtr & factory = this->getFactory();

        BitmapFontPtr font = factory->createObject();

        return font;
    }
}