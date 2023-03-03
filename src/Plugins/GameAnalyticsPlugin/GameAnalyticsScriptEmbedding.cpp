#include "GameAnalyticsScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "GameAnalyticsInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        void s_GameAnalyticsStartProgressionEvent( const Char * _name )
        {
            if( SERVICE_IS_INITIALIZE( GameAnalyticsServiceInterface ) == true )
            {
                GAME_ANALYTICS_SERVICE()
                    ->startProgressionEvent( _name );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void s_GameAnalyticsCompleteProgressionEvent( const Char * _name )
        {
            if( SERVICE_IS_INITIALIZE( GameAnalyticsServiceInterface ) == true )
            {
                GAME_ANALYTICS_SERVICE()
                    ->completeProgressionEvent( _name );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void s_GameAnalyticsFailProgressionEvent( const Char * _name )
        {
            if( SERVICE_IS_INITIALIZE( GameAnalyticsServiceInterface ) == true )
            {
                GAME_ANALYTICS_SERVICE()
                    ->failProgressionEvent( _name );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void s_GameAnalyticsDesignEvent( const Char * _event )
        {
            if( SERVICE_IS_INITIALIZE( GameAnalyticsServiceInterface ) == true )
            {
                GAME_ANALYTICS_SERVICE()
                    ->addDesignEvent( _event );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    GameAnalyticsScriptEmbedding::GameAnalyticsScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GameAnalyticsScriptEmbedding::~GameAnalyticsScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameAnalyticsScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "GameAnalytics", true );

        pybind::def_function( _kernel, "GameAnalyticsStartProgressionEvent", &Detail::s_GameAnalyticsStartProgressionEvent );
        pybind::def_function( _kernel, "GameAnalyticsCompleteProgressionEvent", &Detail::s_GameAnalyticsCompleteProgressionEvent );
        pybind::def_function( _kernel, "GameAnalyticsFailProgressionEvent", &Detail::s_GameAnalyticsFailProgressionEvent );
        pybind::def_function( _kernel, "GameAnalyticsDesignEvent", &Detail::s_GameAnalyticsDesignEvent );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}

