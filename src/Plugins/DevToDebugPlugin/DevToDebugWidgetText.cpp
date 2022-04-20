#include "DevToDebugWidgetText.h"

#include "DevToDebugProperty.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetText::DevToDebugWidgetText()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetText::~DevToDebugWidgetText()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::setDataProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property )
    {
        m_dataProperties.change( _name, _property );
    }
    //////////////////////////////////////////////////////////////////////////
    const DevToDebugPropertyInterfacePtr & DevToDebugWidgetText::getDataProperty( const ConstString & _name ) const
    {
        const DevToDebugPropertyInterfacePtr & property = m_dataProperties.find( _name );

        return property;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::_fillTypeJson( jpp::object & _jdata )
    {
        _jdata.set( "type", "text" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugWidgetText::_fillDataJson( jpp::object & _jdata, bool _force )
    {
        bool invalidate = false;

        for( const HashtableDataProperties::value_type & value : m_dataProperties )
        {
            DevToDebugPropertyPtr property = DevToDebugPropertyPtr::from( value.element );

            invalidate |= property->fillPropertyJson( value.key, _jdata, _force );
        }

        return invalidate;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::process( const jpp::object & _data )
    {
        MENGINE_UNUSED( _data );

        //Emty
    }
    //////////////////////////////////////////////////////////////////////////
}