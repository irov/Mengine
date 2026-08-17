#pragma once

#include "Interface/LayoutBoxInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Updatable.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Vector.h"

#include "math/vec2.h"
#include "math/vec4.h"

#include "Kernel/Viewport.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class EUILayout2DMode
    {
        UIL2DM_RELATIVE,
        UIL2DM_HORIZONTAL,
        UIL2DM_VERTICAL,
    };
    //////////////////////////////////////////////////////////////////////////
    struct UILayout2DConstraints
    {
        UILayout2DConstraints();

        mt::vec2f anchorMin;
        mt::vec2f anchorMax;
        mt::vec2f pivot;
        mt::vec2f position;
        mt::vec2f size;
        mt::vec2f percentPosition;
        mt::vec2f percentSize;
        mt::vec4f margin;
        float flex;
    };
    //////////////////////////////////////////////////////////////////////////
    class UILayout2DItemGetterInterface
        : public Mixin
    {
    public:
        virtual bool onGetUILayout2DItemSize( mt::vec2f * const _size ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UILayout2DItemGetterInterface> UILayout2DItemGetterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UILayout2D;
    //////////////////////////////////////////////////////////////////////////
    class UILayout2DElement
        : public Factorable
    {
        DECLARE_FACTORABLE( UILayout2DElement );

    public:
        UILayout2DElement();
        ~UILayout2DElement() override;

    public:
        void initialize( UILayout2D * _layout, UniqueId _id );
        void finalize();

    public:
        void setAnchors( const mt::vec2f & _min, const mt::vec2f & _max );
        void setPivot( const mt::vec2f & _pivot );
        void setPosition( const mt::vec2f & _position );
        void setSize( const mt::vec2f & _size );
        void setPercentPosition( const mt::vec2f & _position );
        void setPercentSize( const mt::vec2f & _size );
        void setMargin( const mt::vec4f & _margin );
        void setFlex( float _flex );

        UILayout2DConstraints getConstraints() const;
        mt::vec2f getLayoutOffset() const;
        mt::vec2f getLayoutSize() const;

    protected:
        UILayout2D * m_layout;
        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UILayout2DElement> UILayout2DElementPtr;
    //////////////////////////////////////////////////////////////////////////
    class UILayout2DContainer
        : public UILayout2DElement
    {
        DECLARE_FACTORABLE( UILayout2DContainer );

    public:
        UILayout2DContainer();
        ~UILayout2DContainer() override;

    public:
        void setMode( EUILayout2DMode _mode );
        EUILayout2DMode getMode() const;

        void setPadding( const mt::vec4f & _padding );
        mt::vec4f getPadding() const;

        void setSpacing( float _spacing );
        float getSpacing() const;

    public:
        UILayout2DElementPtr addItem( const UILayout2DItemGetterInterfacePtr & _getter, const LayoutBoxElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc );
        IntrusivePtr<UILayout2DContainer> addBox( EUILayout2DMode _mode, const DocumentInterfacePtr & _doc );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UILayout2DContainer> UILayout2DContainerPtr;
    //////////////////////////////////////////////////////////////////////////
    class UILayout2D
        : public Factorable
        , public Updatable
        , protected BaseUpdation
    {
        DECLARE_FACTORABLE( UILayout2D );
        DECLARE_UPDATABLE();

    public:
        UILayout2D();
        ~UILayout2D() override;

    public:
        bool initialize();
        void finalize();

    public:
        void setSizer( const LayoutBoxSizerInterfacePtr & _sizer );
        const UILayout2DContainerPtr & getRoot() const;

        void setUseSafeArea( bool _useSafeArea );
        bool getUseSafeArea() const;

        void invalidate();
        void flush();

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        friend class UILayout2DElement;
        friend class UILayout2DContainer;

        enum class EUILayout2DElementType
        {
            UIL2DET_ITEM,
            UIL2DET_BOX,
        };

        struct ElementDesc
        {
            UniqueId id;
            EUILayout2DElementType type;
            EUILayout2DMode mode;
            UILayout2DConstraints constraints;
            mt::vec4f padding;
            float spacing;
            mt::vec2f intrinsicSize;
            mt::vec2f resolvedOffset;
            mt::vec2f resolvedSize;
            UILayout2DItemGetterInterfacePtr getter;
            LayoutBoxElementSetterInterfacePtr setter;
            Vector<UniqueId> children;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<ElementDesc> VectorElements;

    protected:
        ElementDesc * findElement_( UniqueId _id );
        const ElementDesc * findElement_( UniqueId _id ) const;

        void setElementConstraints_( UniqueId _id, const UILayout2DConstraints & _constraints );
        UILayout2DConstraints getElementConstraints_( UniqueId _id ) const;
        mt::vec2f getElementOffset_( UniqueId _id ) const;
        mt::vec2f getElementSize_( UniqueId _id ) const;

        void setContainerMode_( UniqueId _id, EUILayout2DMode _mode );
        EUILayout2DMode getContainerMode_( UniqueId _id ) const;
        void setContainerPadding_( UniqueId _id, const mt::vec4f & _padding );
        mt::vec4f getContainerPadding_( UniqueId _id ) const;
        void setContainerSpacing_( UniqueId _id, float _spacing );
        float getContainerSpacing_( UniqueId _id ) const;

        UILayout2DElementPtr addItem_( UniqueId _parentId, const UILayout2DItemGetterInterfacePtr & _getter, const LayoutBoxElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc );
        UILayout2DContainerPtr addBox_( UniqueId _parentId, EUILayout2DMode _mode, const DocumentInterfacePtr & _doc );

        UniqueId addElement_( UniqueId _parentId, ElementDesc && _element );
        UILayout2DElementPtr createItemHandle_( UniqueId _id, const DocumentInterfacePtr & _doc );
        UILayout2DContainerPtr createContainerHandle_( UniqueId _id, const DocumentInterfacePtr & _doc );

        void resolveBox_( ElementDesc * const _box, const mt::vec2f & _offset, const mt::vec2f & _size );
        void resolveRelative_( ElementDesc * const _box, const mt::vec2f & _offset, const mt::vec2f & _size );
        void resolveFlow_( ElementDesc * const _box, const mt::vec2f & _offset, const mt::vec2f & _size, bool _horizontal );
        void resolveChild_( ElementDesc * const _child, const mt::vec2f & _offset, const mt::vec2f & _size );

    protected:
        mt::vec2f m_cacheSize;
        Viewport m_cacheSafeAreaViewport;
        LayoutBoxSizerInterfacePtr m_sizer;
        VectorElements m_elements;
        Vector<UILayout2DElementPtr> m_handles;
        UILayout2DContainerPtr m_root;
        bool m_invalidateLayout;
        bool m_useSafeArea;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UILayout2D> UILayout2DPtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        UILayout2DPtr createUILayout2D( const LayoutBoxSizerInterfacePtr & _sizer, const DocumentInterfacePtr & _doc );
        void destroyUILayout2D( const UILayout2DPtr & _layout );
    }
    //////////////////////////////////////////////////////////////////////////
}
