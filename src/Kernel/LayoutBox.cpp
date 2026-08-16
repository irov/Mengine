#include "LayoutBox.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FactorableUnique.h"

#include "Config/StdUtility.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LayoutBoxContainer::LayoutBoxContainer()
        : m_layoutBox( nullptr )
        , m_id( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutBoxContainer::~LayoutBoxContainer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBoxContainer::initialize( LayoutBox * _layoutBox, UniqueId _id )
    {
        m_layoutBox = _layoutBox;
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBoxContainer::finalize()
    {
        m_layoutBox = nullptr;
        m_id = INVALID_UNIQUE_ID;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBoxContainer::addFixed( const LayoutBoxElementGetterInterfacePtr & _getter, const LayoutBoxElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layoutBox, "layout box container is finalized" );

        m_layoutBox->addFixed_( m_id, _getter, _setter, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBoxContainer::addPadding( float _weight, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layoutBox, "layout box container is finalized" );

        m_layoutBox->addPadding_( m_id, _weight, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutBoxContainerInterfacePtr LayoutBoxContainer::addBox( ELayoutBoxDirection _direction, float _size, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layoutBox, "layout box container is finalized" );

        LayoutBoxContainerInterfacePtr container = m_layoutBox->addBox_( m_id, _direction, _size, _doc );

        return container;
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutBox::LayoutBox()
        : m_cacheSize( 0.f, 0.f )
        , m_rootId( INVALID_UNIQUE_ID )
        , m_invalidateLayout( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutBox::~LayoutBox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutBox::initialize()
    {
        UpdationInterface * updation = this->getUpdation();

        updation->activate( EUM_SERVICE_BEFORE, MENGINE_UINT32_C(0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBox::finalize()
    {
        UpdationInterface * updation = this->getUpdation();

        updation->deactivate();

        for( const LayoutBoxContainerPtr & container : m_containers )
        {
            container->finalize();
        }

        m_containers.clear();
        m_elements.clear();

        m_rootId = INVALID_UNIQUE_ID;
        m_sizer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBox::setSizer( const LayoutBoxSizerInterfacePtr & _sizer )
    {
        if( m_sizer == _sizer )
        {
            return;
        }

        m_sizer = _sizer;

        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutBoxContainerInterfacePtr LayoutBox::createRoot( ELayoutBoxDirection _direction, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( m_rootId == INVALID_UNIQUE_ID, "layout box root already exists" );
        MENGINE_ASSERTION_FATAL( m_elements.empty() == true, "layout box elements already exist" );

        LayoutBoxElement root;
        root.id = MENGINE_UINT32_C(1);
        root.type = ELayoutBoxElementType::LBET_BOX;
        root.direction = _direction;
        root.cacheSize = 0.f;
        root.weight = 0.f;
        root.getter = nullptr;
        root.setter = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        root.doc = _doc;
#endif

        m_rootId = root.id;

        m_elements.emplace_back( StdUtility::move( root ) );

        m_invalidateLayout = true;

        LayoutBoxContainerInterfacePtr container = this->createContainer_( m_rootId, _doc );

        return container;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBox::addFixed_( UniqueId _parentId, const LayoutBoxElementGetterInterfacePtr & _getter, const LayoutBoxElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _getter != nullptr, "layout box fixed getter is nullptr" );

        LayoutBoxElement element;
        element.id = INVALID_UNIQUE_ID;
        element.type = ELayoutBoxElementType::LBET_FIXED;
        element.direction = ELayoutBoxDirection::ELBD_HORIZONTAL;
        element.cacheSize = 0.f;
        element.weight = 0.f;
        element.getter = _getter;
        element.setter = _setter;

#if defined(MENGINE_DOCUMENT_ENABLE)
        element.doc = _doc;
#endif

        this->addElement_( _parentId, StdUtility::move( element ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBox::addPadding_( UniqueId _parentId, float _weight, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        LayoutBoxElement element;
        element.id = INVALID_UNIQUE_ID;
        element.type = ELayoutBoxElementType::LBET_PADDING;
        element.direction = ELayoutBoxDirection::ELBD_HORIZONTAL;
        element.cacheSize = 0.f;
        element.weight = _weight;
        element.getter = nullptr;
        element.setter = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        element.doc = _doc;
#endif

        this->addElement_( _parentId, StdUtility::move( element ) );
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutBoxContainerInterfacePtr LayoutBox::addBox_( UniqueId _parentId, ELayoutBoxDirection _direction, float _size, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        LayoutBoxElement element;
        element.id = INVALID_UNIQUE_ID;
        element.type = ELayoutBoxElementType::LBET_BOX;
        element.direction = _direction;
        element.cacheSize = _size;
        element.weight = 0.f;
        element.getter = nullptr;
        element.setter = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        element.doc = _doc;
#endif

        UniqueId id = this->addElement_( _parentId, StdUtility::move( element ) );

        LayoutBoxContainerInterfacePtr container = this->createContainer_( id, _doc );

        return container;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBox::invalidate()
    {
        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBox::flush()
    {
        MENGINE_ASSERTION_FATAL( m_sizer != nullptr, "layout box sizer is not set" );

        if( m_rootId == INVALID_UNIQUE_ID )
        {
            return;
        }

        if( m_sizer->onGetLayoutBoxSize( &m_cacheSize ) == true )
        {
            m_invalidateLayout = true;
        }

        for( LayoutBoxElement & element : m_elements )
        {
            if( element.type != ELayoutBoxElementType::LBET_FIXED )
            {
                continue;
            }

            if( element.getter->onGetLayoutBoxElementSize( &element.cacheSize ) == true )
            {
                m_invalidateLayout = true;
            }
        }

        if( m_invalidateLayout == false )
        {
            return;
        }

        m_invalidateLayout = false;

        const LayoutBoxElement * root = this->findElement_( m_rootId );

        MENGINE_ASSERTION_MEMORY_PANIC( root, "layout box root is not found" );

        this->resolveBox_( *root, mt::vec2f( 0.f, 0.f ), m_cacheSize );
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBox::update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        this->flush();
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutBox::LayoutBoxElement * LayoutBox::findElement_( UniqueId _id )
    {
        if( _id == INVALID_UNIQUE_ID )
        {
            return nullptr;
        }

        size_t index = (size_t)(_id - 1);

        if( index >= m_elements.size() )
        {
            return nullptr;
        }

        LayoutBoxElement & element = m_elements[index];

        if( element.id != _id )
        {
            return nullptr;
        }

        return &element;
    }
    //////////////////////////////////////////////////////////////////////////
    const LayoutBox::LayoutBoxElement * LayoutBox::findElement_( UniqueId _id ) const
    {
        if( _id == INVALID_UNIQUE_ID )
        {
            return nullptr;
        }

        size_t index = (size_t)(_id - 1);

        if( index >= m_elements.size() )
        {
            return nullptr;
        }

        const LayoutBoxElement & element = m_elements[index];

        if( element.id != _id )
        {
            return nullptr;
        }

        return &element;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId LayoutBox::addElement_( UniqueId _parentId, LayoutBoxElement && _element )
    {
        LayoutBoxElement * parent = this->findElement_( _parentId );

        MENGINE_ASSERTION_MEMORY_PANIC( parent, "layout box parent '%u' is not found", _parentId );
        MENGINE_ASSERTION_FATAL( parent->type == ELayoutBoxElementType::LBET_BOX, "layout box parent '%u' is not a box", _parentId );

        UniqueId id = (UniqueId)m_elements.size() + MENGINE_UINT32_C(1);

        _element.id = id;

        m_elements.emplace_back( StdUtility::move( _element ) );

        parent = this->findElement_( _parentId );
        parent->children.emplace_back( id );

        m_invalidateLayout = true;

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutBoxContainerInterfacePtr LayoutBox::createContainer_( UniqueId _id, const DocumentInterfacePtr & _doc )
    {
        LayoutBoxContainerPtr container = Helper::makeFactorableUnique<LayoutBoxContainer>( _doc );
        container->initialize( this, _id );

        m_containers.emplace_back( container );

        return container;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutBox::resolveBox_( const LayoutBoxElement & _box, const mt::vec2f & _offset, const mt::vec2f & _size )
    {
        float totalFixed = 0.f;
        float totalWeight = 0.f;

        for( UniqueId childId : _box.children )
        {
            const LayoutBoxElement * child = this->findElement_( childId );

            MENGINE_ASSERTION_MEMORY_PANIC( child, "layout box child '%u' is not found", childId );

            if( child->type == ELayoutBoxElementType::LBET_PADDING )
            {
                totalWeight += child->weight;
            }
            else
            {
                totalFixed += child->cacheSize;
            }
        }

        float boxMainSize = _box.direction == ELayoutBoxDirection::ELBD_HORIZONTAL
            ? _size.x
            : _size.y;

        float totalAdjusted = boxMainSize > totalFixed
            ? boxMainSize - totalFixed
            : 0.f;

        float carriage = 0.f;

        for( UniqueId childId : _box.children )
        {
            const LayoutBoxElement * child = this->findElement_( childId );

            MENGINE_ASSERTION_MEMORY_PANIC( child, "layout box child '%u' is not found", childId );

            float elementSize = 0.f;

            if( child->type == ELayoutBoxElementType::LBET_PADDING )
            {
                if( totalWeight > 0.f )
                {
                    elementSize = totalAdjusted * child->weight / totalWeight;
                }
            }
            else
            {
                elementSize = child->cacheSize;
            }

            mt::vec2f elementOffset;
            mt::vec2f elementRectSize;

            if( _box.direction == ELayoutBoxDirection::ELBD_HORIZONTAL )
            {
                elementOffset.set( _offset.x + carriage, _offset.y );
                elementRectSize.set( elementSize, _size.y );
            }
            else
            {
                elementOffset.set( _offset.x, _offset.y + carriage );
                elementRectSize.set( _size.x, elementSize );
            }

            if( child->type == ELayoutBoxElementType::LBET_BOX )
            {
                this->resolveBox_( *child, elementOffset, elementRectSize );
            }
            else if( child->type == ELayoutBoxElementType::LBET_FIXED && child->setter != nullptr )
            {
                child->setter->onSetLayoutBoxElementPosition( elementOffset, elementRectSize );
            }

            carriage += elementSize;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
