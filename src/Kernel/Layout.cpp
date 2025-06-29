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
    void Layout::addElement( const ConstString & _name, ELayoutElementType _type, const LayoutElementGetterInterfacePtr & _getter, const LayoutElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( _getter != nullptr, "element '%s' getter is nullptr", _name.c_str() );
        MENGINE_ASSERTION_FATAL( _name.empty() == true || this->hasElement( _name ) == false, "element '%s' already exists in layout", _name.c_str() );

        LayoutElement element;
        element.name = _name;
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
    void Layout::addSubLayout( const ConstString & _name, ELayoutElementType _type, const LayoutInterfacePtr & _subLayout, const LayoutElementGetterInterfacePtr & _getter, const LayoutElementSetterInterfacePtr & _setter, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( _subLayout != nullptr, "sub layout is nullptr" );
        MENGINE_ASSERTION_FATAL( _getter != nullptr, "element '%s' getter is nullptr", _name.c_str() );
        MENGINE_ASSERTION_FATAL( _name.empty() == true || this->hasElement( _name ) == false, "element '%s' already exists in layout", _name.c_str() );

        LayoutElement element;
        element.name = _name;
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
    void Layout::removeElement( const ConstString & _name )
    {
        MENGINE_ASSERTION_FATAL( _name.empty() == false, "element name is empty" );
        MENGINE_ASSERTION_FATAL( this->hasElement( _name ) == true, "element '%s' not found in layout", _name.c_str() );

        VectorLayoutElements::iterator it_found = StdAlgorithm::find_if( m_elements.begin(), m_elements.end(), [_name]( const LayoutElement & element )
        {
            return element.name == _name;
        } );

        if( it_found == m_elements.end() )
        {
            return;
        }

        m_elements.erase( it_found );

        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Layout::hasElement( const ConstString & _name ) const
    {
        for( const LayoutElement & element : m_elements )
        {
            if( element.name != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::clearElements()
    {
        m_elements.clear();

        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::invalidate()
    {
        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

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
}
