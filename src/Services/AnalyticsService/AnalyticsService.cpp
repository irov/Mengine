#include "AnalyticsService.h"

#include "AnalyticsFactory.h"
#include "AnalyticsEventBuilder.h"

#include "Kernel/Logger.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionContainer.h"

#include "Config/Algorithm.h"

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
        VectorAnalyticsEventProvider::iterator it_found = Algorithm::find( m_eventProviders.begin(), m_eventProviders.end(), _provider );

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
    void AnalyticsService::logFlush()
    {
        for( const AnalyticsEventProviderInterfacePtr & provider : m_eventProviders )
        {
            provider->onAnalyticsFlush();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logEarnVirtualCurrency( const ConstString & _currencyName, double _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        AnalyticsContextInterfacePtr context = m_analyticsFactory->makeContext( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_EARN_VIRTUAL_CURRENCY );
        
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@VIRTUAL_CURRENCY_NAME" ), _currencyName );
        builder->addParameterDouble( STRINGIZE_STRING_LOCAL( "@VALUE" ), _value );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logSpendVirtualCurrency( const ConstString & _itemName, const ConstString & _currencyName, double _value, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_SPEND_VIRTUAL_CURRENCY );
        
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@ITEM_NAME" ), _itemName );
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@VIRTUAL_CURRENCY_NAME" ), _currencyName );
        builder->addParameterDouble( STRINGIZE_STRING_LOCAL( "@VALUE" ), _value );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logUnlockAchievement( const ConstString & _achievementId, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_UNLOCK_ACHIEVEMENT );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@ACHIEVEMENT_ID" ), _achievementId );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logLevelUp( const ConstString & _character, int64_t _level, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_LEVEL_UP );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@CHARACTER" ), _character );
        builder->addParameterInteger( STRINGIZE_STRING_LOCAL( "@LEVEL" ), _level );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logLevelStart( const ConstString & _name, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_LEVEL_START );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@LEVEL_NAME" ), _name );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logLevelEnd( const ConstString & _name, bool _successful, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_LEVEL_END );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@LEVEL_NAME" ), _name );
        builder->addParameterBoolean( STRINGIZE_STRING_LOCAL( "@SUCCESS" ), _successful );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logSelectItem( const ConstString & _category, const ConstString & _itemId, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_SELECT_ITEM );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@ITEM_LIST_ID" ), _category );
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@ITEM_LIST_NAME" ), _itemId );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logTutorialBegin( const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_TUTORIAL_BEGIN );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logTutorialComplete( const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_TUTORIAL_COMPLETE );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    void AnalyticsService::logScreenView( const ConstString & _screenClass, const ConstString & _screenName, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_SCREEN_VIEW );

        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@SCREEN_CLASS" ), _screenClass );
        builder->addParameterConstString( STRINGIZE_STRING_LOCAL( "@SCREEN_NAME" ), _screenName );

        builder->log();
    }
    //////////////////////////////////////////////////////////////////////////
    AnalyticsEventBuilderInterfacePtr AnalyticsService::buildEvent( const ConstString & _eventName, const DocumentInterfacePtr & _doc )
    {
        AnalyticsEventBuilderPtr builder = m_analyticsFactory->makeEventBuilder( _doc );

        builder->setAnalyticsFactory( m_analyticsFactory );
        builder->setGlobalContext( m_analyticsGlobalContext );
        builder->setEventType( EAET_CUSTOM );
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
