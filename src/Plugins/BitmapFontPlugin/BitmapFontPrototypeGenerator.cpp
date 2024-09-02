#include "BitmapFontPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/EnumeratorHelper.h"

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
    FactoryInterfacePtr BitmapFontPrototypeGenerator::_initializeFactory()
    {
        FactoryInterfacePtr factory = Helper::makeFactoryPool<BitmapFont, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontPrototypeGenerator::_finalizeFactory()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer BitmapFontPrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        const FactoryInterfacePtr & factory = this->getPrototypeFactory();

        BitmapFontPtr font = factory->createObject( _doc );

        UniqueId uniqueIdentity = Helper::generateUniqueIdentity();
        font->setUniqueIdentity( uniqueIdentity );

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr doc = MENGINE_DOCUMENT_MESSAGE( "BitmapFont '%s' type '%s' create '%s'"
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