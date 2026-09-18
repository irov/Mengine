#pragma once

#include "Kernel/NodePrototypeGenerator.h"

#include "Graphics.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GraphicsPrototypeGenerator
        : public NodePrototypeGenerator<Graphics, 128>
    {
    public:
        GraphicsPrototypeGenerator();
        ~GraphicsPrototypeGenerator() override;

    public:
        void setGraphics( gp_graphics_t * _graphics );
        gp_graphics_t * getGraphics() const;

    protected:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override;

    protected:
        gp_graphics_t * m_graphics;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GraphicsPrototypeGenerator> GraphicsPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}
