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
    void Layout::setLayoutSizer( const LayoutSizerInterfacePtr & _sizer )
    {
        if( m_sizer == _sizer )
        {
            return;
        }

        m_sizer = _sizer;

        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::addLayoutElement( const ConstString & _name, const LayoutElementGetterInterfacePtr & _getter, const LayoutElementSetterInterfacePtr & _setter, bool _fixed, float _weight, bool _enable, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( _fixed == true || _weight > 0.f, "element '%s' flexible weight must be greater than 0.0", _name.c_str() );
        MENGINE_ASSERTION_FATAL( _fixed == false || _weight == 0.f, "element '%s' fixed weight must be 0.0", _name.c_str() );
        MENGINE_ASSERTION_FATAL( _name.empty() == true || this->hasLayoutElement( _name ) == false, "element '%s' already exists in layout", _name.c_str() );

        LayoutElement element;
        element.name = _name;
        element.cacheSize = 0.f;
        element.weight = _weight;
        element.getter = _getter;
        element.setter = _setter;
        element.fixed = _fixed;
        element.enable = _enable;

#if defined(MENGINE_DOCUMENT_ENABLE)
        element.doc = _doc;
#endif

        m_elements.push_back( element );

        m_invalidateLayout = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::removeLayoutElement( const ConstString & _name )
    {
        MENGINE_ASSERTION_FATAL( _name.empty() == false, "element name is empty" );
        MENGINE_ASSERTION_FATAL( this->hasLayoutElement( _name ) == true, "element '%s' not found in layout", _name.c_str() );

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
    bool Layout::hasLayoutElement( const ConstString & _name ) const
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
    void Layout::setLayoutElementEnable( const ConstString & _name, bool _enable )
    {
        MENGINE_ASSERTION_FATAL( _name.empty() == false, "element name is empty" );
        MENGINE_ASSERTION_FATAL( this->hasLayoutElement( _name ) == true, "element '%s' not found in layout", _name.c_str() );

        for( LayoutElement & element : m_elements )
        {
            if( element.name != _name )
            {
                continue;
            }

            element.enable = _enable;

            m_invalidateLayout = true;

            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Layout::isLayoutElementEnable( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_FATAL( _name.empty() == false, "element name is empty" );
        MENGINE_ASSERTION_FATAL( this->hasLayoutElement( _name ) == true, "element '%s' not found in layout", _name.c_str() );

        for( const LayoutElement & element : m_elements )
        {
            if( element.name != _name )
            {
                continue;
            }

            bool enable = element.enable;

            return enable;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layout::update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        MENGINE_ASSERTION_FATAL( m_sizer != nullptr, "layout sizer is not set" );

        if( m_sizer->onGetLayoutSize( &m_cacheSize ) == true )
        {
            m_invalidateLayout = true;
        }

        for( LayoutElement & element : m_elements )
        {
            if( element.enable == false )
            {
                continue;
            }

            if( element.getter == nullptr )
            {
                continue;
            }

            if( element.getter->onGetLayoutElementSize( &element.cacheSize ) == true )
            {
                m_invalidateLayout = true;
            }
        }

        if( m_invalidateLayout == false )
        {
            return;
        }

        m_invalidateLayout = false;

        if( m_elements.empty() == true )
        {
            return;
        }

        bool anyEnabled = StdAlgorithm::any_of( m_elements.begin(), m_elements.end(), []( const LayoutElement & element )
        {
            return element.enable;
        } );

        if( anyEnabled == false )
        {
            return;
        }

        float totalFixed = 0.f;
        float totalWeight = 0.f;

        for( const LayoutElement & element : m_elements )
        {
            if( element.enable == false )
            {
                continue;
            }

            if( element.fixed == true )
            {
                totalFixed += element.cacheSize;
            }
            else
            {
                totalWeight += element.weight;
            }
        }

        float totalAdjusted = m_cacheSize > totalFixed ? m_cacheSize - totalFixed : 0.f;

        float carriage = 0.f;

        for( const LayoutElement & element : m_elements )
        {
            if( element.enable == false )
            {
                continue;
            }

            float elementSize = 0;

            if( element.fixed == true )
            {
                elementSize = element.cacheSize;
            }
            else if( totalWeight > 0.f )
            {
                float flexibleSize = totalAdjusted * element.weight / totalWeight;

                elementSize = flexibleSize;
            }

            if( element.setter != nullptr )
            {
                element.setter->onSetLayoutElementPosition( carriage, elementSize );
            }

            carriage += elementSize;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
