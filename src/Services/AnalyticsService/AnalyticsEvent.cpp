#include "AnalyticsEvent.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEvent::AnalyticsEvent()
        : m_type( EAET_CUSTOM )
        , m_timestamp( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEvent::~AnalyticsEvent()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEvent::setType( EAnalyticsEventType _type )
    {
        m_type = _type;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventType AnalyticsEvent::getType() const
    {
        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEvent::setName( const ConstString & _name )
    {
        m_name = _name;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & AnalyticsEvent::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEvent::setContext( const AnalyticsContextInterfacePtr & _context )
    {
        m_context = _context;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsContextInterfacePtr & AnalyticsEvent::getContext() const
    {
        return m_context;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEvent::setTimestamp( Timestamp _timestamp )
    {
        m_timestamp = _timestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp AnalyticsEvent::getTimestamp() const
    {
        return m_timestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEvent::addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
    {
        AnalyticsEventParameterDesc desc;
        desc.name = _name;
        desc.parameter = _parameter;

        m_parameters.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AnalyticsEvent::getCountParameters() const
    {
        VectorAnalyticsEventParameter::size_type parameters_size = m_parameters.size();

        return (uint32_t)parameters_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEvent::foreachParameters( const LambdaEventParameter & _lambda ) const
    {
        for( const AnalyticsEventParameterDesc & desc : m_parameters )
        {
            const ConstString & name = desc.name;
            const AnalyticsEventParameterInterfacePtr & parameter = desc.parameter;

            _lambda( name, parameter );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}