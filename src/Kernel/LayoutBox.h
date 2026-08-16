#pragma once

#include "Interface/LayoutBoxInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Updatable.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LayoutBox;
    //////////////////////////////////////////////////////////////////////////
    class LayoutBoxContainer
        : public LayoutBoxContainerInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( LayoutBoxContainer );

    public:
        LayoutBoxContainer();
        ~LayoutBoxContainer() override;

    public:
        void initialize( LayoutBox * _layoutBox, UniqueId _id );
        void finalize();

    public:
        void addFixed( const LayoutBoxElementGetterInterfacePtr & _getter, const LayoutBoxElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc ) override;
        void addPadding( float _weight, const DocumentInterfacePtr & _doc ) override;
        LayoutBoxContainerInterfacePtr addBox( ELayoutBoxDirection _direction, float _size, const DocumentInterfacePtr & _doc ) override;

    protected:
        LayoutBox * m_layoutBox;
        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutBoxContainer, LayoutBoxContainerInterface> LayoutBoxContainerPtr;
    //////////////////////////////////////////////////////////////////////////
    class LayoutBox
        : public LayoutBoxInterface
        , public Updatable
        , protected BaseUpdation
    {
        DECLARE_FACTORABLE( LayoutBox );
        DECLARE_UPDATABLE();

    public:
        LayoutBox();
        ~LayoutBox() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setSizer( const LayoutBoxSizerInterfacePtr & _sizer ) override;

    public:
        LayoutBoxContainerInterfacePtr createRoot( ELayoutBoxDirection _direction, const DocumentInterfacePtr & _doc ) override;

    public:
        void invalidate() override;
        void flush() override;

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        friend class LayoutBoxContainer;

        void addFixed_( UniqueId _parentId, const LayoutBoxElementGetterInterfacePtr & _getter, const LayoutBoxElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc );
        void addPadding_( UniqueId _parentId, float _weight, const DocumentInterfacePtr & _doc );
        LayoutBoxContainerInterfacePtr addBox_( UniqueId _parentId, ELayoutBoxDirection _direction, float _size, const DocumentInterfacePtr & _doc );

        enum class ELayoutBoxElementType
        {
            LBET_BOX,
            LBET_FIXED,
            LBET_PADDING,
        };

        struct LayoutBoxElement
        {
            UniqueId id;
            ELayoutBoxElementType type;
            ELayoutBoxDirection direction;

            float cacheSize;
            float weight;

            LayoutBoxElementGetterInterfacePtr getter;
            LayoutBoxElementSetterInterfacePtr setter;

            Vector<UniqueId> children;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<LayoutBoxElement> VectorLayoutBoxElements;

    protected:
        LayoutBoxElement * findElement_( UniqueId _id );
        const LayoutBoxElement * findElement_( UniqueId _id ) const;

        UniqueId addElement_( UniqueId _parentId, LayoutBoxElement && _element );
        LayoutBoxContainerInterfacePtr createContainer_( UniqueId _id, const DocumentInterfacePtr & _doc );
        void resolveBox_( const LayoutBoxElement & _box, const mt::vec2f & _offset, const mt::vec2f & _size );

    protected:
        mt::vec2f m_cacheSize;
        LayoutBoxSizerInterfacePtr m_sizer;

        VectorLayoutBoxElements m_elements;
        Vector<LayoutBoxContainerPtr> m_containers;

        UniqueId m_rootId;

        bool m_invalidateLayout;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutBox, LayoutBoxInterface> LayoutBoxPtr;
    //////////////////////////////////////////////////////////////////////////
}
