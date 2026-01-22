#pragma once

#include "ImGUIInterface.h"

#include "Kernel/NodePrototypeGenerator.h"

#include "ImGUIRender.h"

namespace Mengine
{
    class ImGUIRenderPrototypeGenerator
        : public NodePrototypeGenerator<ImGUIRender, 16>
    {
    public:
        ImGUIRenderPrototypeGenerator();
        ~ImGUIRenderPrototypeGenerator() override;

    public:
        void setImGUIRenderProvider( const ImGUIRenderProviderInterfacePtr & _renderProvider );
        const ImGUIRenderProviderInterfacePtr & getImGUIRenderProvider() const;

    protected:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override;

    protected:
        ImGUIRenderProviderInterfacePtr m_renderProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImGUIRenderPrototypeGenerator, PrototypeGeneratorInterface> ImGUIRenderPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}
