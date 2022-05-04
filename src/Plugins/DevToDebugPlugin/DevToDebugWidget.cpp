#include "DevToDebugWidget.h"

#include "DevToDebugProperty.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidget::DevToDebugWidget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidget::~DevToDebugWidget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidget::setId( const ConstString & _id )
    {
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DevToDebugWidget::getId() const
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidget::setBaseProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property )
    {
        m_baseProperties.change( _name, _property );
    }
    //////////////////////////////////////////////////////////////////////////
    const DevToDebugPropertyInterfacePtr & DevToDebugWidget::getBaseProperty( const ConstString & _name ) const
    {
        const DevToDebugPropertyInterfacePtr & property = m_baseProperties.find( _name );

        return property;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidget::syncProperties()
    {
        for( const HashtableBaseProperties::value_type & value : m_baseProperties )
        {
            const DevToDebugPropertyInterfacePtr & property = value.element;

            property->sync();
        }

        this->_syncPropertis();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugWidget::fillJson( jpp::object & _jwidget, bool _force )
    {
        MENGINE_ASSERTION_FATAL( m_id.empty() == false, "DevToDebug widget type '%s' miss id"
            , this->getType().c_str()
        );

        _jwidget.set( "id", m_id );

        this->_fillTypeJson( _jwidget );

        bool invalidate_property = false;

        for( const HashtableBaseProperties::value_type & value : m_baseProperties )
        {
            DevToDebugPropertyPtr property = DevToDebugPropertyPtr::from( value.element );

            invalidate_property |= property->fillPropertyJson( value.key, _jwidget, _force );
        }

        jpp::object jdata = jpp::make_object();

        bool invalidate_data = this->_fillDataJson( jdata, _force );

        if( invalidate_data == true )
        {
            _jwidget.set( "data", jdata );
        }

        return invalidate_property | invalidate_data;
    }
    //////////////////////////////////////////////////////////////////////////
}