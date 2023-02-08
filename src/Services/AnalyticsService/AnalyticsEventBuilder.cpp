#include "AnalyticsEventBuilder.h"

#include "Kernel/TimeHelper.h"

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
    void AnalyticsEventBuilder::addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
    {
        AnalyticsEventParameterDesc desc;
        desc.name = _name;
        desc.parameter = _parameter;

        m_parameters.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterface * AnalyticsEventBuilder::addParameterBoolean( const ConstString & _name, bool _value )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventConstParameterBoolean( _value );

        this->addParameter( _name, parameter );

        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterface * AnalyticsEventBuilder::addParameterConstString( const ConstString & _name, const ConstString & _value )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventConstParameterConstString( _value );

        this->addParameter( _name, parameter );

        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterface * AnalyticsEventBuilder::addParameterString( const ConstString & _name, const String & _value )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventConstParameterString( _value );

        this->addParameter( _name, parameter );

        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterface * AnalyticsEventBuilder::addParameterInteger( const ConstString & _name, int64_t _value )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventConstParameterInteger( _value );

        this->addParameter( _name, parameter );

        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterface * AnalyticsEventBuilder::addParameterDouble( const ConstString & _name, double _value )
    {
        AnalyticsEventParameterInterfacePtr parameter = m_analyticsFactory->makeAnalyticsEventConstParameterDouble( _value );

        this->addParameter( _name, parameter );

        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsEventBuilder::log()
    {
        AnalyticsEventInterfacePtr event = m_analyticsFactory->makeAnalyticsEvent( m_eventType, m_eventName );

        for( const AnalyticsEventParameterDesc & desc : m_parameters )
        {
            const ConstString & name = desc.name;
            const AnalyticsEventParameterInterfacePtr & parameter = desc.parameter;

            event->addParameter( name, parameter );
        }

        m_parameters.clear();

        AnalyticsContextInterfacePtr resolve_context = m_globalContext->resolveContext();

        event->setContext( resolve_context );

        TimeMilliseconds timestamp = Helper::getTimeMilliseconds();
        event->setTimestamp( timestamp );

        m_analyticsFactory = nullptr;
        m_globalContext = nullptr;

        ANALYTICS_SERVICE()
            ->logEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
}