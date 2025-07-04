#include "Layout.h"

#include "Kernel/Assertion.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Layout::Layout()
        : m_cacheSize( 0.f )
        , m_invalidateLayout( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Layout::~Layout()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Layout::initialize()
    {
        UpdationInterface * updation = this->getUpdation();

        updation->activate( EUM_SERVICE_BEFORE, 0U );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::finalize()
    {
        UpdationInterface * updation = this->getUpdation();

        updation->deactivate();

        for( LayoutElement & el : m_elements )
        {
            if( el.subLayout != nullptr )
            {
                el.subLayout->finalize();
            }
        }

        m_elements.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::setSizer( const LayoutSizerInterfacePtr & _sizer )
    {
        if( m_sizer == _sizer )
        {
            return;
        }

        m_sizer = _sizer;

        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::addElement( ELayoutElementType _type, const LayoutElementGetterInterfacePtr & _getter, const LayoutElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( _getter != nullptr, "element getter is nullptr" );

        LayoutElement element;
        element.subLayout = nullptr;
        element.type = _type;
        element.cacheValue = 0.f;
        element.getter = _getter;
        element.setter = _setter;

#if defined(MENGINE_DOCUMENT_ENABLE)
        element.doc = _doc;
#endif

        m_elements.push_back( element );

        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::addSubLayout( ELayoutElementType _type, const LayoutInterfacePtr & _subLayout, const LayoutElementGetterInterfacePtr & _getter, const LayoutElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( _subLayout != nullptr, "sub layout is nullptr" );
        MENGINE_ASSERTION_FATAL( _getter != nullptr, "element getter is nullptr" );

        LayoutElement element;
        element.subLayout = _subLayout;
        element.type = _type;
        element.cacheValue = 0.f;
        element.getter = _getter;
        element.setter = _setter;

#if defined(MENGINE_DOCUMENT_ENABLE)
        element.doc = _doc;
#endif

        m_elements.push_back( element );

        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::invalidate()
    {
        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::flush()
    {
        MENGINE_ASSERTION_FATAL( m_sizer != nullptr, "layout sizer is not set" );

        if( m_elements.empty() == true )
        {
            return;
        }

        if( m_sizer->onGetLayoutSize( &m_cacheSize ) == true )
        {
            m_invalidateLayout = true;
        }

        for( LayoutElement & element : m_elements )
        {
            if( element.getter == nullptr )
            {
                continue;
            }

            if( element.getter->onGetLayoutElementSize( &element.cacheValue ) == true )
            {
                m_invalidateLayout = true;
            }
        }

        if( m_invalidateLayout == false )
        {
            return;
        }

        m_invalidateLayout = false;

        float totalFixed = 0.f;
        float totalWeight = 0.f;

        for( const LayoutElement & element : m_elements )
        {
            switch( element.type )
            {
            case ELayoutElementType::LET_FIXED:
                {
                    totalFixed += element.cacheValue;
                }break;
            case ELayoutElementType::LET_PAD:
                {
                    totalWeight += element.cacheValue;
                }break;
            }
        }

        float totalAdjusted = m_cacheSize > totalFixed ? m_cacheSize - totalFixed : 0.f;

        float carriage = 0.f;

        for( const LayoutElement & element : m_elements )
        {
            float elementSize = 0.f;

            switch( element.type )
            {
            case ELayoutElementType::LET_FIXED:
                {
                    elementSize = element.cacheValue;
                }break;
            case ELayoutElementType::LET_PAD:
                {
                    if( totalWeight > 0.f )
                    {
                        float flexibleSize = totalAdjusted * element.cacheValue / totalWeight;

                        elementSize = flexibleSize;
                    }
                }break;
            }

            if( element.setter != nullptr )
            {
                element.setter->onSetLayoutElementPosition( carriage, elementSize );
            }

            if( element.subLayout != nullptr )
            {
                element.subLayout->invalidate();
            }

            carriage += elementSize;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        this->flush();
    }
    //////////////////////////////////////////////////////////////////////////
}
