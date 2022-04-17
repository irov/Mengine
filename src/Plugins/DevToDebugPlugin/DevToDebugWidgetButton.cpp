#include "DevToDebugWidgetButton.h"

#include "DevToDebugProperty.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetButton::DevToDebugWidgetButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetButton::~DevToDebugWidgetButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::setDataProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property )
    {
        m_dataProperties.emplace( _name, _property );
    }
    //////////////////////////////////////////////////////////////////////////
    const DevToDebugPropertyInterfacePtr & DevToDebugWidgetButton::getDataProperty( const ConstString & _name ) const
    {
        const DevToDebugPropertyInterfacePtr & property = m_dataProperties.find( _name );

        return property;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::setClickEvent( const LambdaClickEvent & _clickEvent )
    {
        m_clickEvent = _clickEvent;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::_fillTypeJson( jpp::object & _jdata )
    {
        _jdata.set( "type", "button" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugWidgetButton::_fillDataJson( jpp::object & _jdata, bool _force )
    {
        bool invalidate = false;

        for( const HashtableDataProperties::value_type & value : m_dataProperties )
        {
            DevToDebugPropertyPtr property = DevToDebugPropertyPtr::from( value.element );

            invalidate |= property->fillPropertyJson( value.key, _jdata, _force );
        }

        return invalidate;
    }
    ////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::process( const jpp::object & _data )
    {
        MENGINE_UNUSED( _data );

        if( m_clickEvent == nullptr )
        {
            return;
        }

        m_clickEvent();
    }
    ////////////////////////////////////////////////////////////////////////
}