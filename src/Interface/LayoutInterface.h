#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Viewport.h"

namespace Mengine
{
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
    class LayoutInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void setLayoutSizer( const LayoutSizerInterfacePtr & _sizer ) = 0;

    public:
        virtual void addLayoutElement( const ConstString & _name, const LayoutElementGetterInterfacePtr & _getter, const LayoutElementSetterInterfacePtr & _setter, bool _fixed, float _weight, bool _enable, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeLayoutElement( const ConstString & _name ) = 0;
        virtual bool hasLayoutElement( const ConstString & _name ) const = 0;

    public:
        virtual void setLayoutElementEnable( const ConstString & _name, bool _enable ) = 0;
        virtual bool isLayoutElementEnable( const ConstString & _name ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutInterface> LayoutInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}