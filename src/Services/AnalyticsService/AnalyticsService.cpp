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
        
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@VIRTUAL_CURRENCY_NAME" ), _currencyName );
        builder->addParameterDouble( STRINGIZE_STRING_LOCAL( "@VALUE" ), _value );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logSpendVirtualCurrency( const ConstString & _itemName, const ConstString & _currencyName, double _value )
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_SPEND_VIRTUAL_CURRENCY );
        
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@ITEM_NAME" ), _itemName );
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@VIRTUAL_CURRENCY_NAME" ), _currencyName );
        builder->addParameterDouble( STRINGIZE_STRING_LOCAL( "@VALUE" ), _value );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logUnlockAchievement( const ConstString & _achievementId )
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_UNLOCK_ACHIEVEMENT );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@ACHIEVEMENT_ID" ), _achievementId );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logLevelUp( const ConstString & _character, int64_t _level )
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_LEVEL_UP );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@CHARACTER" ), _character );
        builder->addParameterInteger( STRINGIZE_STRING_LOCAL( "@LEVEL" ), _level );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logLevelStart( const ConstString & _name )
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_LEVEL_START );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@LEVEL_NAME" ), _name );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logLevelEnd( const ConstString & _name, bool _successful )
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_LEVEL_END );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@LEVEL_NAME" ), _name );
        builder->addParameterBoolean( STRINGIZE_STRING_LOCAL( "@SUCCESS" ), _successful );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logSelectItem( const ConstString & _category, const ConstString & _itemId )
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_SELECT_ITEM );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@ITEM_LIST_ID" ), _category );
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@ITEM_LIST_NAME" ), _itemId );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logTutorialBegin()
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_TUTORIAL_BEGIN );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logTutorialComplete()
    {
        AnalyticsEventBuilderPtr builder = m_factoryAnalyticsEventBuilder->createObject( MENGINE_DOCUMENT_FACTORABLE );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_TUTORIAL_COMPLETE );

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
