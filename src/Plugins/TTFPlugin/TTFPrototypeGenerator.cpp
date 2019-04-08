#include "TTFPrototypeGenerator.h"

#include "Kernel/FactoryPool.h"

#include "TTFFont.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFPrototypeGenerator::TTFPrototypeGenerator()
        : m_ftlibrary( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFPrototypeGenerator::~TTFPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFPrototypeGenerator::setFTLibrary( FT_Library _ftlibrary )
    {
        m_ftlibrary = _ftlibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryPtr TTFPrototypeGenerator::_initializeFactory()
    {
        FactoryPtr factory = Helper::makeFactoryPool<TTFFont, 8>();

        return factory;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer TTFPrototypeGenerator::generate( const Char * _doc )
    {
        const FactoryPtr & factory = this->getFactory();

        TTFFontPtr font = factory->createObject( _doc );

        font->setFTLibrary( m_ftlibrary );

        return font;
    }
}