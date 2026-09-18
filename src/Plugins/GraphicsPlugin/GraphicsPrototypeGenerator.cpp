#include "GraphicsPrototypeGenerator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GraphicsPrototypeGenerator::GraphicsPrototypeGenerator()
        : m_graphics( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsPrototypeGenerator::~GraphicsPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPrototypeGenerator::setGraphics( gp_graphics_t * _graphics )
    {
        m_graphics = _graphics;
    }
    //////////////////////////////////////////////////////////////////////////
    gp_graphics_t * GraphicsPrototypeGenerator::getGraphics() const
    {
        return m_graphics;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer GraphicsPrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        GraphicsPtr graphics = NodePrototypeGenerator<Graphics, 128>::generate( _doc );

        graphics->createCanvas( m_graphics );

        return graphics;
    }
    //////////////////////////////////////////////////////////////////////////
}
