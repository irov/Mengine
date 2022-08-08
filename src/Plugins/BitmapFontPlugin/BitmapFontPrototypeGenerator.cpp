#include "BitmapFontPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/DocumentHelper.h"

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
        FactoryPtr factory = Helper::makeFactoryPool<BitmapFont, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontPrototypeGenerator::_finalizeFactory()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer BitmapFontPrototypeGenerator::generate( const DocumentPtr & _doc )
    {
        const FactoryPtr & factory = this->getPrototypeFactory();

        BitmapFontPtr font = factory->createObject( _doc );

#ifdef MENGINE_DOCUMENT_ENABLE
        DocumentPtr doc = MENGINE_DOCUMENT_MESSAGE( "Sprite '%s' type '%s' create '%s'"
            , font->getName().c_str()
            , font->getType().c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        font->setDocument( doc );
#endif

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
}