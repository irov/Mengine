#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/IntrusivePtr.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class ELayoutBoxDirection
    {
        ELBD_HORIZONTAL,
        ELBD_VERTICAL,
    };
    //////////////////////////////////////////////////////////////////////////
    class LayoutBoxSizerInterface
        : public Mixin
    {
    public:
        virtual bool onGetLayoutBoxSize( mt::vec2f * const _size ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutBoxSizerInterface> LayoutBoxSizerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class LayoutBoxElementGetterInterface
        : public Mixin
    {
    public:
        virtual bool onGetLayoutBoxElementSize( float * const _size ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutBoxElementGetterInterface> LayoutBoxElementGetterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class LayoutBoxElementSetterInterface
        : public Mixin
    {
    public:
        virtual void onSetLayoutBoxElementPosition( const mt::vec2f & _offset, const mt::vec2f & _size ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutBoxElementSetterInterface> LayoutBoxElementSetterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class LayoutBoxContainerInterface;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutBoxContainerInterface> LayoutBoxContainerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class LayoutBoxContainerInterface
        : public Mixin
    {
    public:
        virtual void addFixed( const LayoutBoxElementGetterInterfacePtr & _getter, const LayoutBoxElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc ) = 0;
        virtual void addPadding( float _weight, const DocumentInterfacePtr & _doc ) = 0;
        virtual LayoutBoxContainerInterfacePtr addBox( ELayoutBoxDirection _direction, float _size, const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class LayoutBoxInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void setSizer( const LayoutBoxSizerInterfacePtr & _sizer ) = 0;

    public:
        virtual LayoutBoxContainerInterfacePtr createRoot( ELayoutBoxDirection _direction, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual void invalidate() = 0;
        virtual void flush() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutBoxInterface> LayoutBoxInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
