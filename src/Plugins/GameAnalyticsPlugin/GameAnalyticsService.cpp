#include "GameAnalyticsService.h"

#include "GameAnalytics.h"

#include "Kernel/NotificationHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GameAnalyticsService, Mengine::GameAnalyticsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GameAnalyticsService::GameAnalyticsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GameAnalyticsService::~GameAnalyticsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameAnalyticsService::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME, &GameAnalyticsService::notifyBootstrapperInitializeGame_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME, &GameAnalyticsService::notifyBootstrapperFinalizeGame_, MENGINE_DOCUMENT_FACTORABLE );

        ANALYTICS_SERVICE()
            ->addEventProvider( AnalyticsEventProviderInterfacePtr::from( this ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );

        ANALYTICS_SERVICE()
            ->removeEventProvider( AnalyticsEventProviderInterfacePtr::from( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::startProgressionEvent( const Char * _name )
    {
        gameanalytics::GameAnalytics::addProgressionEvent( gameanalytics::EGAProgressionStatus::Start, _name, "", "" );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::completeProgressionEvent( const Char * _name )
    {
        gameanalytics::GameAnalytics::addProgressionEvent( gameanalytics::EGAProgressionStatus::Complete, _name, "", "" );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::failProgressionEvent( const Char * _name )
    {
        gameanalytics::GameAnalytics::addProgressionEvent( gameanalytics::EGAProgressionStatus::Fail, _name, "", "" );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::addDesignEvent( const Char * _event )
    {
        gameanalytics::GameAnalytics::addDesignEvent( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::notifyBootstrapperInitializeGame_()
    {
        gameanalytics::GameAnalytics::startSession();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::notifyBootstrapperFinalizeGame_()
    {
        gameanalytics::GameAnalytics::endSession();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {
        const ConstString & eventName = _event->getName();
        const Char * eventName_str = eventName.c_str();

        String json;
        json += "{";
        _event->foreachParameters( [&json]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
        {
            const Char * name_str = _name.c_str();

            json += "\"";
            json += name_str;
            json += "\":";

            EAnalyticsEventParameterType parameterType = _parameter->getType();

            switch( parameterType )
            {
            case EAEPT_BOOLEAN:
                {
                    AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );
                    bool parameter_value = parameter_boolean->resolveValue();

                    if( parameter_value == true )
                    {
                        json += "1";
                    }
                    else
                    {
                        json += "0";
                    }
                }break;
            case EAEPT_INTEGER:
                {
                    AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );
                    int64_t parameter_value = parameter_integer->resolveValue();

                    Char parameter_str[128] = {'\0'};
                    Helper::stringalized( parameter_value, parameter_str, 128 );

                    json += parameter_str;
                }break;
            case EAEPT_DOUBLE:
                {
                    AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );
                    double parameter_value = parameter_double->resolveValue();

                    Char parameter_str[128] = {'\0'};
                    Helper::stringalized( parameter_value, parameter_str, 128 );

                    json += parameter_str;
                }break;
            case EAEPT_STRING:
                {
                    AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );
                    const String & parameter_value = parameter_string->resolveValue();

                    json += "\"";
                    json += parameter_value;
                    json += "\"";
                }break;
            case EAEPT_CONSTSTRING:
                {
                    AnalyticsEventParameterConstStringInterfacePtr parameter_string = AnalyticsEventParameterConstStringInterfacePtr::from( _parameter );
                    const ConstString & parameter_value = parameter_string->resolveValue();

                    json += "\"";
                    json += parameter_value.c_str();
                    json += "\"";
                }break;
            }

            json += ",";
        } );

        uint32_t parametersCount = _event->getCountParameters();

        if( parametersCount != 0 )
        {
            json.pop_back(); //remove comma
        }

        json += "}";

        const Char * json_str = json.c_str();

        LOGGER_INFO( "gameanalytics", "design event: %s %s"
            , eventName_str
            , json_str
        );

        gameanalytics::GameAnalytics::addDesignEvent( eventName_str, json_str );
    }
    //////////////////////////////////////////////////////////////////////////
}
