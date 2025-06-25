#pragma once

#include "Interface/LayoutInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Updatable.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Vector.h"

namespace Mengine
{

    //////////////////////////////////////////////////////////////////////////
    class Layout
        : public LayoutInterface
        , public Updatable
        , protected BaseUpdation
    {
        DECLARE_FACTORABLE( Layout );
        DECLARE_UPDATABLE();

    public:
        Layout();
        ~Layout() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setLayoutSizer( const LayoutSizerInterfacePtr & _sizer ) override;

    public:
        void addLayoutElement( const ConstString & _name, ELayoutElementType _type, const LayoutElementGetterInterfacePtr & _getter, const LayoutElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc ) override;
        void removeLayoutElement( const ConstString & _name ) override;
        bool hasLayoutElement( const ConstString & _name ) const override;

    public:
        void clearLayoutElements() override;

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        float m_cacheSize;
        LayoutSizerInterfacePtr m_sizer;

        struct LayoutElement
        {
            ConstString name;
            ELayoutElementType type;
            float cacheValue;
            LayoutElementGetterInterfacePtr getter;
            LayoutElementSetterInterfacePtr setter;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<LayoutElement> VectorLayoutElements;
        VectorLayoutElements m_elements;

        bool m_invalidateLayout;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Layout, LayoutInterface> LayoutPtr;
    //////////////////////////////////////////////////////////////////////////
}