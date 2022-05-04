#include "DevToDebugWidgetCheckbox.h"

#include "DevToDebugProperty.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetCheckbox::DevToDebugWidgetCheckbox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetCheckbox::~DevToDebugWidgetCheckbox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetCheckbox::setDataProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property )
    {
        m_dataProperties.change( _name, _property );
    }
    //////////////////////////////////////////////////////////////////////////
    const DevToDebugPropertyInterfacePtr & DevToDebugWidgetCheckbox::getDataProperty( const ConstString & _name ) const
    {
        const DevToDebugPropertyInterfacePtr & property = m_dataProperties.find( _name );

        return property;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetCheckbox::setClickEvent( const LambdaClickEvent & _clickEvent )
    {
        m_clickEvent = _clickEvent;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetCheckbox::_syncPropertis()
    {
        for( const HashtableBaseProperties::value_type & value : m_dataProperties )
        {
            const DevToDebugPropertyInterfacePtr & property = value.element;

            property->sync();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetCheckbox::_fillTypeJson( jpp::object & _jdata )
    {
        _jdata.set( "type", "check-box" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugWidgetCheckbox::_fillDataJson( jpp::object & _jdata, bool _force )
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
    void DevToDebugWidgetCheckbox::process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands )
    {
        MENGINE_UNUSED( _data );

        if( m_clickEvent == nullptr )
        {
            return;
        }

        bool value = _data.get( "value" );

        _mutex->lock();
        _commands->emplace_back( [this, value]()
        {
            m_clickEvent( value );
        } );
        _mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
}