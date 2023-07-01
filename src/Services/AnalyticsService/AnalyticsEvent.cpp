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
    void AnalyticsEvent::setLocalContext( const AnalyticsContextInterfacePtr & _localContext )
    {
        m_localContext = _localContext;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsContextInterfacePtr & AnalyticsEvent::getLocalContext() const
    {
        return m_localContext;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEvent::setGlobalContext( const AnalyticsContextInterfacePtr & _globalContext )
    {
        m_globalContext = _globalContext;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsContextInterfacePtr & AnalyticsEvent::getGlobalContext() const
    {
        return m_globalContext;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AnalyticsEvent::getCountParameters() const
    {
        uint32_t parametersCount = 0;

        uint32_t selfParametersCount = m_context->getCountParameters();
        parametersCount += selfParametersCount;

        if( m_localContext != nullptr )
        {
            uint32_t localParametersCount = m_localContext->getCountParameters();
            parametersCount += localParametersCount;
        }

        if( m_globalContext != nullptr )
        {
            uint32_t globalParametersCount = m_globalContext->getCountParameters();
            parametersCount += globalParametersCount;
        }

        return parametersCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEvent::foreachParameters( const LambdaEventParameter & _lambda ) const
    {
        m_context->foreachParameters( _lambda );

        if( m_localContext != nullptr )
        {
            m_localContext->foreachParameters( _lambda );
        }

        if( m_globalContext != nullptr )
        {
            m_globalContext->foreachParameters( _lambda );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}