#include "UILayout2D.h"

#include "Interface/ApplicationInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FactorableUnique.h"

#include "Config/StdUtility.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static float clampNonnegative( float _value )
        {
            return _value > 0.f ? _value : 0.f;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec4f clampInsets( const mt::vec4f & _insets )
        {
            return mt::vec4f(
                clampNonnegative( _insets.x ),
                clampNonnegative( _insets.y ),
                clampNonnegative( _insets.z ),
                clampNonnegative( _insets.w )
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static void makeInsetRect( const mt::vec2f & _offset, const mt::vec2f & _size, const mt::vec4f & _insets, mt::vec2f * const _innerOffset, mt::vec2f * const _innerSize )
        {
            _innerOffset->set( _offset.x + _insets.x, _offset.y + _insets.y );
            _innerSize->set(
                clampNonnegative( _size.x - _insets.x - _insets.z ),
                clampNonnegative( _size.y - _insets.y - _insets.w )
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static float getAxis( const mt::vec2f & _value, bool _horizontal )
        {
            return _horizontal == true ? _value.x : _value.y;
        }
        //////////////////////////////////////////////////////////////////////////
        static float getLeadingMargin( const mt::vec4f & _margin, bool _horizontal )
        {
            return _horizontal == true ? _margin.x : _margin.y;
        }
        //////////////////////////////////////////////////////////////////////////
        static float getTrailingMargin( const mt::vec4f & _margin, bool _horizontal )
        {
            return _horizontal == true ? _margin.z : _margin.w;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DConstraints::UILayout2DConstraints()
        : anchorMin( 0.f, 0.f )
        , anchorMax( 0.f, 0.f )
        , pivot( 0.f, 0.f )
        , position( 0.f, 0.f )
        , size( -1.f, -1.f )
        , percentPosition( 0.f, 0.f )
        , percentSize( 0.f, 0.f )
        , margin( 0.f, 0.f, 0.f, 0.f )
        , flex( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DElement::UILayout2DElement()
        : m_layout( nullptr )
        , m_id( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DElement::~UILayout2DElement()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DElement::initialize( UILayout2D * _layout, UniqueId _id )
    {
        m_layout = _layout;
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DElement::finalize()
    {
        m_layout = nullptr;
        m_id = INVALID_UNIQUE_ID;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DElement::setAnchors( const mt::vec2f & _min, const mt::vec2f & _max )
    {
        UILayout2DConstraints constraints = this->getConstraints();
        constraints.anchorMin = _min;
        constraints.anchorMax = _max;

        m_layout->setElementConstraints_( m_id, constraints );
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DElement::setPivot( const mt::vec2f & _pivot )
    {
        UILayout2DConstraints constraints = this->getConstraints();
        constraints.pivot = _pivot;

        m_layout->setElementConstraints_( m_id, constraints );
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DElement::setPosition( const mt::vec2f & _position )
    {
        UILayout2DConstraints constraints = this->getConstraints();
        constraints.position = _position;

        m_layout->setElementConstraints_( m_id, constraints );
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DElement::setSize( const mt::vec2f & _size )
    {
        UILayout2DConstraints constraints = this->getConstraints();
        constraints.size = _size;

        m_layout->setElementConstraints_( m_id, constraints );
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DElement::setPercentPosition( const mt::vec2f & _position )
    {
        UILayout2DConstraints constraints = this->getConstraints();
        constraints.percentPosition = _position;

        m_layout->setElementConstraints_( m_id, constraints );
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DElement::setPercentSize( const mt::vec2f & _size )
    {
        UILayout2DConstraints constraints = this->getConstraints();
        constraints.percentSize = _size;

        m_layout->setElementConstraints_( m_id, constraints );
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DElement::setMargin( const mt::vec4f & _margin )
    {
        UILayout2DConstraints constraints = this->getConstraints();
        constraints.margin = Detail::clampInsets( _margin );

        m_layout->setElementConstraints_( m_id, constraints );
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DElement::setFlex( float _flex )
    {
        UILayout2DConstraints constraints = this->getConstraints();
        constraints.flex = Detail::clampNonnegative( _flex );

        m_layout->setElementConstraints_( m_id, constraints );
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DConstraints UILayout2DElement::getConstraints() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout element is finalized" );

        return m_layout->getElementConstraints_( m_id );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f UILayout2DElement::getLayoutOffset() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout element is finalized" );

        return m_layout->getElementOffset_( m_id );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f UILayout2DElement::getLayoutSize() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout element is finalized" );

        return m_layout->getElementSize_( m_id );
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DContainer::UILayout2DContainer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DContainer::~UILayout2DContainer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DContainer::setMode( EUILayout2DMode _mode )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout container is finalized" );

        m_layout->setContainerMode_( m_id, _mode );
    }
    //////////////////////////////////////////////////////////////////////////
    EUILayout2DMode UILayout2DContainer::getMode() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout container is finalized" );

        return m_layout->getContainerMode_( m_id );
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DContainer::setPadding( const mt::vec4f & _padding )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout container is finalized" );

        m_layout->setContainerPadding_( m_id, Detail::clampInsets( _padding ) );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec4f UILayout2DContainer::getPadding() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout container is finalized" );

        return m_layout->getContainerPadding_( m_id );
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2DContainer::setSpacing( float _spacing )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout container is finalized" );

        m_layout->setContainerSpacing_( m_id, Detail::clampNonnegative( _spacing ) );
    }
    //////////////////////////////////////////////////////////////////////////
    float UILayout2DContainer::getSpacing() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout container is finalized" );

        return m_layout->getContainerSpacing_( m_id );
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DElementPtr UILayout2DContainer::addItem( const UILayout2DItemGetterInterfacePtr & _getter, const LayoutBoxElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout container is finalized" );

        return m_layout->addItem_( m_id, _getter, _setter, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DContainerPtr UILayout2DContainer::addBox( EUILayout2DMode _mode, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_layout, "UI layout container is finalized" );

        return m_layout->addBox_( m_id, _mode, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2D::UILayout2D()
        : m_cacheSize( 0.f, 0.f )
        , m_invalidateLayout( true )
        , m_useSafeArea( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2D::~UILayout2D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UILayout2D::initialize()
    {
        MENGINE_ASSERTION_FATAL( m_elements.empty() == true, "UI layout is already initialized" );

        ElementDesc root;
        root.id = MENGINE_UINT32_C(1);
        root.type = EUILayout2DElementType::UIL2DET_BOX;
        root.mode = EUILayout2DMode::UIL2DM_RELATIVE;
        root.padding.set( 0.f, 0.f, 0.f, 0.f );
        root.spacing = 0.f;
        root.intrinsicSize.set( 0.f, 0.f );
        root.resolvedOffset.set( 0.f, 0.f );
        root.resolvedSize.set( 0.f, 0.f );
        root.getter = nullptr;
        root.setter = nullptr;

        m_elements.emplace_back( StdUtility::move( root ) );

        m_root = this->createContainerHandle_( MENGINE_UINT32_C(1), MENGINE_DOCUMENT_FACTORABLE );

        UpdationInterface * updation = this->getUpdation();
        updation->activate( EUM_SERVICE_BEFORE, MENGINE_UINT32_C(0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::finalize()
    {
        UpdationInterface * updation = this->getUpdation();
        updation->deactivate();

        for( const UILayout2DElementPtr & handle : m_handles )
        {
            handle->finalize();
        }

        m_root = nullptr;
        m_handles.clear();
        m_elements.clear();
        m_sizer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::setSizer( const LayoutBoxSizerInterfacePtr & _sizer )
    {
        if( m_sizer == _sizer )
        {
            return;
        }

        m_sizer = _sizer;
        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const UILayout2DContainerPtr & UILayout2D::getRoot() const
    {
        return m_root;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::setUseSafeArea( bool _useSafeArea )
    {
        if( m_useSafeArea == _useSafeArea )
        {
            return;
        }

        m_useSafeArea = _useSafeArea;
        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UILayout2D::getUseSafeArea() const
    {
        return m_useSafeArea;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::invalidate()
    {
        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::flush()
    {
        MENGINE_ASSERTION_FATAL( m_sizer != nullptr, "UI layout sizer is not set" );

        if( m_sizer->onGetLayoutBoxSize( &m_cacheSize ) == true )
        {
            m_invalidateLayout = true;
        }

        Viewport safeAreaViewport;

        if( m_useSafeArea == true && SERVICE_IS_INITIALIZE( ApplicationInterface ) == true )
        {
            safeAreaViewport = APPLICATION_SERVICE()
                ->getSafeAreaViewport();
        }

        if( m_cacheSafeAreaViewport != safeAreaViewport )
        {
            m_cacheSafeAreaViewport = safeAreaViewport;
            m_invalidateLayout = true;
        }

        for( ElementDesc & element : m_elements )
        {
            if( element.type != EUILayout2DElementType::UIL2DET_ITEM || element.getter == nullptr )
            {
                continue;
            }

            if( element.getter->onGetUILayout2DItemSize( &element.intrinsicSize ) == true )
            {
                m_invalidateLayout = true;
            }
        }

        if( m_invalidateLayout == false )
        {
            return;
        }

        m_invalidateLayout = false;

        ElementDesc * root = this->findElement_( MENGINE_UINT32_C(1) );
        MENGINE_ASSERTION_MEMORY_PANIC( root, "UI layout root is not found" );

        mt::vec2f rootOffset( 0.f, 0.f );
        mt::vec2f rootSize = m_cacheSize;

        if( m_useSafeArea == true && m_cacheSafeAreaViewport.empty() == false )
        {
            const Resolution & windowResolution = APPLICATION_SERVICE()
                ->getCurrentWindowResolution();

            float windowWidth = windowResolution.getWidthF();
            float windowHeight = windowResolution.getHeightF();

            if( windowWidth <= 0.f || windowHeight <= 0.f )
            {
                this->resolveBox_( root, rootOffset, rootSize );

                return;
            }

            float scaleX = m_cacheSize.x / windowWidth;
            float scaleY = m_cacheSize.y / windowHeight;

            rootOffset.x = m_cacheSafeAreaViewport.begin.x * scaleX;
            rootOffset.y = m_cacheSafeAreaViewport.begin.y * scaleY;
            rootSize.x = m_cacheSafeAreaViewport.getWidth() * scaleX;
            rootSize.y = m_cacheSafeAreaViewport.getHeight() * scaleY;
        }

        this->resolveBox_( root, rootOffset, rootSize );
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        this->flush();
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2D::ElementDesc * UILayout2D::findElement_( UniqueId _id )
    {
        if( _id == INVALID_UNIQUE_ID )
        {
            return nullptr;
        }

        size_t index = (size_t)(_id - MENGINE_UINT32_C(1));

        if( index >= m_elements.size() )
        {
            return nullptr;
        }

        ElementDesc & element = m_elements[index];

        return element.id == _id ? &element : nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const UILayout2D::ElementDesc * UILayout2D::findElement_( UniqueId _id ) const
    {
        if( _id == INVALID_UNIQUE_ID )
        {
            return nullptr;
        }

        size_t index = (size_t)(_id - MENGINE_UINT32_C(1));

        if( index >= m_elements.size() )
        {
            return nullptr;
        }

        const ElementDesc & element = m_elements[index];

        return element.id == _id ? &element : nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::setElementConstraints_( UniqueId _id, const UILayout2DConstraints & _constraints )
    {
        ElementDesc * element = this->findElement_( _id );
        MENGINE_ASSERTION_MEMORY_PANIC( element, "UI layout element '%u' is not found", _id );

        element->constraints = _constraints;
        element->constraints.margin = Detail::clampInsets( element->constraints.margin );
        element->constraints.flex = Detail::clampNonnegative( element->constraints.flex );

        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DConstraints UILayout2D::getElementConstraints_( UniqueId _id ) const
    {
        const ElementDesc * element = this->findElement_( _id );
        MENGINE_ASSERTION_MEMORY_PANIC( element, "UI layout element '%u' is not found", _id );

        return element->constraints;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f UILayout2D::getElementOffset_( UniqueId _id ) const
    {
        const ElementDesc * element = this->findElement_( _id );
        MENGINE_ASSERTION_MEMORY_PANIC( element, "UI layout element '%u' is not found", _id );

        return element->resolvedOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f UILayout2D::getElementSize_( UniqueId _id ) const
    {
        const ElementDesc * element = this->findElement_( _id );
        MENGINE_ASSERTION_MEMORY_PANIC( element, "UI layout element '%u' is not found", _id );

        return element->resolvedSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::setContainerMode_( UniqueId _id, EUILayout2DMode _mode )
    {
        ElementDesc * element = this->findElement_( _id );
        MENGINE_ASSERTION_MEMORY_PANIC( element, "UI layout container '%u' is not found", _id );
        MENGINE_ASSERTION_FATAL( element->type == EUILayout2DElementType::UIL2DET_BOX, "UI layout element '%u' is not a box", _id );

        if( element->mode == _mode )
        {
            return;
        }

        element->mode = _mode;
        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    EUILayout2DMode UILayout2D::getContainerMode_( UniqueId _id ) const
    {
        const ElementDesc * element = this->findElement_( _id );
        MENGINE_ASSERTION_MEMORY_PANIC( element, "UI layout container '%u' is not found", _id );

        return element->mode;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::setContainerPadding_( UniqueId _id, const mt::vec4f & _padding )
    {
        ElementDesc * element = this->findElement_( _id );
        MENGINE_ASSERTION_MEMORY_PANIC( element, "UI layout container '%u' is not found", _id );

        if( element->padding == _padding )
        {
            return;
        }

        element->padding = _padding;
        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec4f UILayout2D::getContainerPadding_( UniqueId _id ) const
    {
        const ElementDesc * element = this->findElement_( _id );
        MENGINE_ASSERTION_MEMORY_PANIC( element, "UI layout container '%u' is not found", _id );

        return element->padding;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::setContainerSpacing_( UniqueId _id, float _spacing )
    {
        ElementDesc * element = this->findElement_( _id );
        MENGINE_ASSERTION_MEMORY_PANIC( element, "UI layout container '%u' is not found", _id );

        if( element->spacing == _spacing )
        {
            return;
        }

        element->spacing = _spacing;
        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    float UILayout2D::getContainerSpacing_( UniqueId _id ) const
    {
        const ElementDesc * element = this->findElement_( _id );
        MENGINE_ASSERTION_MEMORY_PANIC( element, "UI layout container '%u' is not found", _id );

        return element->spacing;
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DElementPtr UILayout2D::addItem_( UniqueId _parentId, const UILayout2DItemGetterInterfacePtr & _getter, const LayoutBoxElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc )
    {
        ElementDesc element;
        element.id = INVALID_UNIQUE_ID;
        element.type = EUILayout2DElementType::UIL2DET_ITEM;
        element.mode = EUILayout2DMode::UIL2DM_RELATIVE;
        element.padding.set( 0.f, 0.f, 0.f, 0.f );
        element.spacing = 0.f;
        element.intrinsicSize.set( 0.f, 0.f );
        element.resolvedOffset.set( 0.f, 0.f );
        element.resolvedSize.set( 0.f, 0.f );
        element.getter = _getter;
        element.setter = _setter;

#if defined(MENGINE_DOCUMENT_ENABLE)
        element.doc = _doc;
#endif

        UniqueId id = this->addElement_( _parentId, StdUtility::move( element ) );

        return this->createItemHandle_( id, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DContainerPtr UILayout2D::addBox_( UniqueId _parentId, EUILayout2DMode _mode, const DocumentInterfacePtr & _doc )
    {
        ElementDesc element;
        element.id = INVALID_UNIQUE_ID;
        element.type = EUILayout2DElementType::UIL2DET_BOX;
        element.mode = _mode;
        element.padding.set( 0.f, 0.f, 0.f, 0.f );
        element.spacing = 0.f;
        element.intrinsicSize.set( 0.f, 0.f );
        element.resolvedOffset.set( 0.f, 0.f );
        element.resolvedSize.set( 0.f, 0.f );
        element.getter = nullptr;
        element.setter = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        element.doc = _doc;
#endif

        UniqueId id = this->addElement_( _parentId, StdUtility::move( element ) );

        return this->createContainerHandle_( id, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId UILayout2D::addElement_( UniqueId _parentId, ElementDesc && _element )
    {
        ElementDesc * parent = this->findElement_( _parentId );
        MENGINE_ASSERTION_MEMORY_PANIC( parent, "UI layout parent '%u' is not found", _parentId );
        MENGINE_ASSERTION_FATAL( parent->type == EUILayout2DElementType::UIL2DET_BOX, "UI layout parent '%u' is not a box", _parentId );

        UniqueId id = (UniqueId)m_elements.size() + MENGINE_UINT32_C(1);
        _element.id = id;

        m_elements.emplace_back( StdUtility::move( _element ) );

        parent = this->findElement_( _parentId );
        parent->children.emplace_back( id );

        m_invalidateLayout = true;

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DElementPtr UILayout2D::createItemHandle_( UniqueId _id, const DocumentInterfacePtr & _doc )
    {
        UILayout2DElementPtr handle = Helper::makeFactorableUnique<UILayout2DElement>( _doc );
        handle->initialize( this, _id );

        m_handles.emplace_back( handle );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    UILayout2DContainerPtr UILayout2D::createContainerHandle_( UniqueId _id, const DocumentInterfacePtr & _doc )
    {
        UILayout2DContainerPtr handle = Helper::makeFactorableUnique<UILayout2DContainer>( _doc );
        handle->initialize( this, _id );

        m_handles.emplace_back( handle );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::resolveBox_( ElementDesc * const _box, const mt::vec2f & _offset, const mt::vec2f & _size )
    {
        _box->resolvedOffset = _offset;
        _box->resolvedSize = _size;

        mt::vec2f innerOffset;
        mt::vec2f innerSize;
        Detail::makeInsetRect( _offset, _size, _box->padding, &innerOffset, &innerSize );

        switch( _box->mode )
        {
        case EUILayout2DMode::UIL2DM_RELATIVE:
            this->resolveRelative_( _box, innerOffset, innerSize );
            break;
        case EUILayout2DMode::UIL2DM_HORIZONTAL:
            this->resolveFlow_( _box, innerOffset, innerSize, true );
            break;
        case EUILayout2DMode::UIL2DM_VERTICAL:
            this->resolveFlow_( _box, innerOffset, innerSize, false );
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::resolveRelative_( ElementDesc * const _box, const mt::vec2f & _offset, const mt::vec2f & _size )
    {
        for( UniqueId childId : _box->children )
        {
            ElementDesc * child = this->findElement_( childId );
            MENGINE_ASSERTION_MEMORY_PANIC( child, "UI layout child '%u' is not found", childId );

            mt::vec2f availableOffset;
            mt::vec2f availableSize;
            Detail::makeInsetRect( _offset, _size, child->constraints.margin, &availableOffset, &availableSize );

            const UILayout2DConstraints & constraints = child->constraints;
            mt::vec2f baseSize = constraints.size;

            if( baseSize.x < 0.f )
            {
                baseSize.x = child->intrinsicSize.x;
            }

            if( baseSize.y < 0.f )
            {
                baseSize.y = child->intrinsicSize.y;
            }

            mt::vec2f anchorSpan = constraints.anchorMax - constraints.anchorMin;
            mt::vec2f childSize = baseSize + availableSize * (anchorSpan + constraints.percentSize);
            childSize.x = Detail::clampNonnegative( childSize.x );
            childSize.y = Detail::clampNonnegative( childSize.y );

            mt::vec2f anchor = constraints.anchorMin + anchorSpan * constraints.pivot + constraints.percentPosition;
            mt::vec2f childOffset = availableOffset + availableSize * anchor + constraints.position - childSize * constraints.pivot;

            this->resolveChild_( child, childOffset, childSize );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::resolveFlow_( ElementDesc * const _box, const mt::vec2f & _offset, const mt::vec2f & _size, bool _horizontal )
    {
        float contentMainSize = Detail::getAxis( _size, _horizontal );
        float fixedMainSize = 0.f;
        float totalFlex = 0.f;

        size_t childCount = _box->children.size();

        if( childCount > 1 )
        {
            fixedMainSize += _box->spacing * (float)(childCount - 1);
        }

        for( UniqueId childId : _box->children )
        {
            const ElementDesc * child = this->findElement_( childId );
            MENGINE_ASSERTION_MEMORY_PANIC( child, "UI layout child '%u' is not found", childId );

            const UILayout2DConstraints & constraints = child->constraints;
            float configuredSize = Detail::getAxis( constraints.size, _horizontal );
            float intrinsicSize = Detail::getAxis( child->intrinsicSize, _horizontal );
            float percentSize = Detail::getAxis( constraints.percentSize, _horizontal );
            float mainSize = (configuredSize >= 0.f ? configuredSize : intrinsicSize) + contentMainSize * percentSize;

            fixedMainSize += Detail::clampNonnegative( mainSize );
            fixedMainSize += Detail::getLeadingMargin( constraints.margin, _horizontal );
            fixedMainSize += Detail::getTrailingMargin( constraints.margin, _horizontal );
            totalFlex += constraints.flex;
        }

        float remaining = contentMainSize > fixedMainSize ? contentMainSize - fixedMainSize : 0.f;
        float carriage = 0.f;

        for( UniqueId childId : _box->children )
        {
            ElementDesc * child = this->findElement_( childId );
            MENGINE_ASSERTION_MEMORY_PANIC( child, "UI layout child '%u' is not found", childId );

            const UILayout2DConstraints & constraints = child->constraints;
            float leadingMargin = Detail::getLeadingMargin( constraints.margin, _horizontal );
            float trailingMargin = Detail::getTrailingMargin( constraints.margin, _horizontal );
            float configuredMain = Detail::getAxis( constraints.size, _horizontal );
            float intrinsicMain = Detail::getAxis( child->intrinsicSize, _horizontal );
            float percentMain = Detail::getAxis( constraints.percentSize, _horizontal );
            float mainSize = Detail::clampNonnegative( (configuredMain >= 0.f ? configuredMain : intrinsicMain) + contentMainSize * percentMain );

            if( totalFlex > 0.f && constraints.flex > 0.f )
            {
                mainSize += remaining * constraints.flex / totalFlex;
            }

            float crossSize = _horizontal == true ? _size.y : _size.x;
            float leadingCrossMargin = _horizontal == true ? constraints.margin.y : constraints.margin.x;
            float trailingCrossMargin = _horizontal == true ? constraints.margin.w : constraints.margin.z;
            float availableCrossSize = Detail::clampNonnegative( crossSize - leadingCrossMargin - trailingCrossMargin );
            float configuredCross = _horizontal == true ? constraints.size.y : constraints.size.x;
            float intrinsicCross = _horizontal == true ? child->intrinsicSize.y : child->intrinsicSize.x;
            float percentCross = _horizontal == true ? constraints.percentSize.y : constraints.percentSize.x;
            float anchorMinCross = _horizontal == true ? constraints.anchorMin.y : constraints.anchorMin.x;
            float anchorMaxCross = _horizontal == true ? constraints.anchorMax.y : constraints.anchorMax.x;
            float pivotCross = _horizontal == true ? constraints.pivot.y : constraints.pivot.x;
            float percentPositionCross = _horizontal == true ? constraints.percentPosition.y : constraints.percentPosition.x;
            float positionCross = _horizontal == true ? constraints.position.y : constraints.position.x;

            float resolvedCrossSize;

            if( configuredCross < 0.f && intrinsicCross <= 0.f && percentCross == 0.f && anchorMinCross == anchorMaxCross )
            {
                resolvedCrossSize = availableCrossSize;
            }
            else
            {
                float baseCross = configuredCross >= 0.f ? configuredCross : intrinsicCross;
                resolvedCrossSize = Detail::clampNonnegative( baseCross + availableCrossSize * (percentCross + anchorMaxCross - anchorMinCross) );
            }

            float anchorCross = anchorMinCross + (anchorMaxCross - anchorMinCross) * pivotCross + percentPositionCross;
            float crossOffset = leadingCrossMargin + availableCrossSize * anchorCross + positionCross - resolvedCrossSize * pivotCross;

            mt::vec2f childOffset;
            mt::vec2f childSize;

            if( _horizontal == true )
            {
                childOffset.set( _offset.x + carriage + leadingMargin + constraints.position.x, _offset.y + crossOffset );
                childSize.set( mainSize, resolvedCrossSize );
            }
            else
            {
                childOffset.set( _offset.x + crossOffset, _offset.y + carriage + leadingMargin + constraints.position.y );
                childSize.set( resolvedCrossSize, mainSize );
            }

            this->resolveChild_( child, childOffset, childSize );

            carriage += leadingMargin + mainSize + trailingMargin + _box->spacing;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UILayout2D::resolveChild_( ElementDesc * const _child, const mt::vec2f & _offset, const mt::vec2f & _size )
    {
        _child->resolvedOffset = _offset;
        _child->resolvedSize = _size;

        if( _child->type == EUILayout2DElementType::UIL2DET_BOX )
        {
            this->resolveBox_( _child, _offset, _size );

            return;
        }

        if( _child->setter != nullptr )
        {
            _child->setter->onSetLayoutBoxElementPosition( _offset, _size );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        UILayout2DPtr createUILayout2D( const LayoutBoxSizerInterfacePtr & _sizer, const DocumentInterfacePtr & _doc )
        {
            UILayout2DPtr layout = Helper::makeFactorableUnique<UILayout2D>( _doc );

            if( layout->initialize() == false )
            {
                return nullptr;
            }

            layout->setSizer( _sizer );

            return layout;
        }
        //////////////////////////////////////////////////////////////////////////
        void destroyUILayout2D( const UILayout2DPtr & _layout )
        {
            if( _layout == nullptr )
            {
                return;
            }

            _layout->finalize();
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
