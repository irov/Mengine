#include "AnalyticsService.h"

#include "AnalyticsFactory.h"
#include "AnalyticsEventBuilder.h"

#include "Kernel/Logger.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionContainer.h"

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

        m_factoryAnalyticsEventBuilder = Helper::makeFactoryPool<AnalyticsEventBuilder, 16>( MENGINE_DOCUMENT_FACTORABLE );

        m_analyticsGlobalContext = m_analyticsFactory->makeAnalyticsContext();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::_finalizeService()
    {
        m_analyticsGlobalContext = nullptr;

        m_analyticsFactory->finalize();
        m_analyticsFactory = nullptr;

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_eventProviders );

        m_eventProviders.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAnalyticsEventBuilder );

        m_factoryAnalyticsEventBuilder = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::addEventProvider( const AnalyticsEventProviderInterfacePtr & _provider )
    {
        m_eventProviders.emplace_back( _provider );
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::removeEventProvider( const AnalyticsEventProviderInterfacePtr & _provider )
    {
        VectorAnalyticsEventProvider::iterator it_found = std::find( m_eventProviders.begin(), m_eventProviders.end(), _provider );

        MENGINE_ASSERTION( it_found != m_eventProviders.end() );

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
    void AnalyticsService::logEarnVirtualCurrency( const ConstString & _currencyName, double _value )
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_EARN_VIRTUAL_CURRENCY );
        
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@INTERNAL_VIRTUAL_CURRENCY_NAME" ), _currencyName );
        builder->addParameterDouble( STRINGIZE_STRING_LOCAL( "@INTERNAL_VALUE" ), _value );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logSpendVirtualCurrency( const ConstString & _itemName, const ConstString & _currencyName, double _value )
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_SPEND_VIRTUAL_CURRENCY );
        
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@INTERNAL_ITEM_NAME" ), _itemName );
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@INTERNAL_VIRTUAL_CURRENCY_NAME" ), _currencyName );
        builder->addParameterDouble( STRINGIZE_STRING_LOCAL( "@INTERNAL_VALUE" ), _value );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logUnlockAchievement( const ConstString & _achievementId )
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_UNLOCK_ACHIEVEMENT );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@INTERNAL_ACHIEVEMENT_ID" ), _achievementId );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterfacePtr AnalyticsService::buildEvent( const ConstString & _eventName )
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_CUSTOM );
        builder->setEventName( _eventName );

        return builder;
    }
    //////////////////////////////////////////////////////////////////////////
    const AnalyticsContextInterfacePtr & AnalyticsService::getGlobalContext() const
    {
        return m_analyticsGlobalContext;
    }
    //////////////////////////////////////////////////////////////////////////
}
