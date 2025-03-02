#include "AnalyticsService.h"

#include "AnalyticsFactory.h"
#include "AnalyticsEventBuilder.h"

#include "Kernel/Logger.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionContainer.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AnalyticsService, Mengine::AnalyticsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AnalyticsService::AnalyticsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsService::~AnalyticsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AnalyticsService::_initializeService()
    {
        m_analyticsFactory = Helper::makeFactorableUnique<AnalyticsFactory>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_analyticsFactory->initialize() == false )
        {
            return false;
        }        

        m_analyticsGlobalContext = m_analyticsFactory->makeContext( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::_finalizeService()
    {
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_eventProviders );
        m_eventProviders.clear();

        m_analyticsGlobalContext = nullptr;

        m_analyticsFactory->finalize();
        m_analyticsFactory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::addEventProvider( const AnalyticsEventProviderInterfacePtr & _provider )
    {
        m_eventProviders.emplace_back( _provider );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::removeEventProvider( const AnalyticsEventProviderInterfacePtr & _provider )
    {
        VectorAnalyticsEventProvider::iterator it_found = StdAlgorithm::find( m_eventProviders.begin(), m_eventProviders.end(), _provider );

        MENGINE_ASSERTION( it_found != m_eventProviders.end(), "event provider not found" );

        m_eventProviders.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logEvent( const AnalyticsEventInterfacePtr & _event )
    {
        for( const AnalyticsEventProviderInterfacePtr & provider : m_eventProviders )
        {
            provider->onAnalyticsEvent( _event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        for( const AnalyticsEventProviderInterfacePtr & provider : m_eventProviders )
        {
            provider->onAnalyticsScreenView( _screenType, _screenName );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logFlush()
    {
        for( const AnalyticsEventProviderInterfacePtr & provider : m_eventProviders )
        {
            provider->onAnalyticsFlush();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterfacePtr AnalyticsService::buildEvent( const ConstString & _eventName, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventName( _eventName );

        return builder;
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsContextInterfacePtr AnalyticsService::makeContext( const DocumentInterfacePtr & _doc )
    {
        AnalyticsContextInterfacePtr analyticsContext = m_analyticsFactory->makeContext( _doc );

        return analyticsContext;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsContextInterfacePtr & AnalyticsService::getGlobalContext() const
    {
        return m_analyticsGlobalContext;
    }
    //////////////////////////////////////////////////////////////////////////
}
