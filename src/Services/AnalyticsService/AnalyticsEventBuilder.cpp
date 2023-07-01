#include "AnalyticsEventBuilder.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilder::AnalyticsEventBuilder()
        : m_eventType( EAET_CUSTOM )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilder::~AnalyticsEventBuilder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventBuilder::setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory )
    {
        m_analyticsFactory = _analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsFactoryInterfacePtr & AnalyticsEventBuilder::getAnalyticsFactory() const
    {
        return m_analyticsFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventBuilder::setContext( const AnalyticsContextInterfacePtr & _context )
    {
        m_context = _context;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsContextInterfacePtr & AnalyticsEventBuilder::getContext() const
    {
        return m_context;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventBuilder::setGlobalContext( const AnalyticsContextInterfacePtr & _globalContext )
    {
        m_globalContext = _globalContext;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsContextInterfacePtr & AnalyticsEventBuilder::getGlobalContext() const
    {
        return m_globalContext;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventBuilder::setEventType( EAnalyticsEventType _eventType )
    {
        m_eventType = _eventType;
    }
    //////////////////////////////////////////////////////////////////////////
    EAnalyticsEventType AnalyticsEventBuilder::getEventType() const
    {
        return m_eventType;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventBuilder::setEventName( const ConstString & _eventName )
    {
        m_eventName = _eventName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & AnalyticsEventBuilder::getEventName() const
    {
        return m_eventName;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventBuilder::setLocalContext( const AnalyticsContextInterfacePtr & _localContext )
    {
        m_localContext = _localContext;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsContextInterfacePtr & AnalyticsEventBuilder::getLocalContext() const
    {
        return m_localContext;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventBuilder::addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
    {
        m_context->addParameter( _name, _parameter );
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterface * AnalyticsEventBuilder::addParameterBoolean( const ConstString & _name, bool _value )
    {
        m_context->addParameterBoolean( _name, _value, MENGINE_DOCUMENT_FACTORABLE );

        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterface * AnalyticsEventBuilder::addParameterConstString( const ConstString & _name, const ConstString & _value )
    {
        m_context->addParameterConstString( _name, _value, MENGINE_DOCUMENT_FACTORABLE );

        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterface * AnalyticsEventBuilder::addParameterString( const ConstString & _name, const String & _value )
    {
        m_context->addParameterString( _name, _value, MENGINE_DOCUMENT_FACTORABLE );

        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterface * AnalyticsEventBuilder::addParameterInteger( const ConstString & _name, int64_t _value )
    {
        m_context->addParameterInteger( _name, _value, MENGINE_DOCUMENT_FACTORABLE );

        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterface * AnalyticsEventBuilder::addParameterDouble( const ConstString & _name, double _value )
    {
        m_context->addParameterDouble( _name, _value, MENGINE_DOCUMENT_FACTORABLE );

        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp AnalyticsEventBuilder::log()
    {
        AnalyticsEventInterfacePtr event = m_analyticsFactory->makeEvent( m_eventType, m_eventName, MENGINE_DOCUMENT_FACTORABLE );

        event->setContext( m_context );

        AnalyticsContextInterfacePtr resolve_globalContext = m_globalContext->resolveContext( MENGINE_DOCUMENT_FACTORABLE );
        event->setGlobalContext( resolve_globalContext );

        if( m_localContext != nullptr )
        {
            AnalyticsContextInterfacePtr resolve_localContext = m_localContext->resolveContext( MENGINE_DOCUMENT_FACTORABLE );
            event->setLocalContext( resolve_localContext );
        }

        Timestamp timestamp = Helper::getTimestamp();
        event->setTimestamp( timestamp );

        ANALYTICS_SERVICE()
            ->logEvent( event );

        return timestamp;
    }
    //////////////////////////////////////////////////////////////////////////
}