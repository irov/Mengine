#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Viewport.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class ELayoutElementType
    {
        LET_FIXED,
        LET_PAD,
    };
    //////////////////////////////////////////////////////////////////////////
    class LayoutSizerInterface
        : public Mixin
    {
    public:
        virtual bool onGetLayoutSize( float * const _size ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutSizerInterface> LayoutSizerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class LayoutElementGetterInterface
        : public Mixin
    {
    public:
        virtual bool onGetLayoutElementSize( float * const _size ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutElementGetterInterface> LayoutElementGetterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class LayoutElementSetterInterface
        : public Mixin
    {
    public:
        virtual void onSetLayoutElementPosition( float _offset, float _size ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutElementSetterInterface> LayoutElementSetterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class LayoutInterface> LayoutInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class LayoutInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void setSizer( const LayoutSizerInterfacePtr & _sizer ) = 0;

    public:
        virtual void addElement( ELayoutElementType _type, const LayoutElementGetterInterfacePtr & _getter, const LayoutElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc ) = 0;
        virtual void addSubLayout( ELayoutElementType _type, const LayoutInterfacePtr & _subLayout, const LayoutElementGetterInterfacePtr & _getter, const LayoutElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual void invalidate() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutInterface> LayoutInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}