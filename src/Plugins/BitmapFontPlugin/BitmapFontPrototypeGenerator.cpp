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
        FactoryPtr factory = Helper::makeFactoryPool<BitmapFont, 8>();

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontPrototypeGenerator::_finalizeFactory()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer BitmapFontPrototypeGenerator::generate( const Char * _doc )
    {
        const FactoryPtr & factory = this->getFactory();

        BitmapFontPtr font = factory->createObject( _doc );

        return font;
    }
}