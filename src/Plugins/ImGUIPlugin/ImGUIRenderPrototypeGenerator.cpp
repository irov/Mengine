#include "ImGUIRenderPrototypeGenerator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImGUIRenderPrototypeGenerator::ImGUIRenderPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIRenderPrototypeGenerator::~ImGUIRenderPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRenderPrototypeGenerator::setImGUIRenderProvider( const ImGUIRenderProviderInterfacePtr & _renderProvider )
    {
        m_renderProvider = _renderProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const ImGUIRenderProviderInterfacePtr & ImGUIRenderPrototypeGenerator::getImGUIRenderProvider() const
    {
        return m_renderProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer ImGUIRenderPrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        ImGUIRenderPtr render = NodePrototypeGenerator<ImGUIRender, 16>::generate( _doc );

        render->setImGUIRenderProvider( m_renderProvider );

        return render;
    }
    //////////////////////////////////////////////////////////////////////////
}