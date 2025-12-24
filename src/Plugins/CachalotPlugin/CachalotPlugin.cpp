#include "CachalotPlugin.h"

#include "Interface/LoggerServiceInterface.h"
#include "Interface/HttpServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/PluginHelper.h"

#include "CachalotLogger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Cachalot, Mengine::CachalotPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CachalotPlugin::CachalotPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CachalotPlugin::~CachalotPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CachalotPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & CachalotPlugin::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( LoggerServiceInterface ),
            SERVICE_ID( HttpServiceInterface ),
            SERVICE_ID( ThreadSystemInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CachalotPlugin::_availablePlugin() const
    {
        bool OPTION_nologs = HAS_OPTION( "nologs" );

        if( OPTION_nologs == true )
        {
            return false;
        }

        bool OPTION_nocachalot = HAS_OPTION( "nocachalot" );

        if( OPTION_nocachalot == true )
        {
            return false;
        }

        if( HAS_CONFIG( "CachalotPlugin", "DSN" ) == false )
        {
            LOGGER_MESSAGE( "Cachalot unavailable [not setup DSN]" );

            return false;
        }

        bool CachalotPlugin_Available = CONFIG_VALUE_BOOLEAN( "CachalotPlugin", "Available", true );

        if( CachalotPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CachalotPlugin::_initializePlugin()
    {
        URLString CachalotPlugin_DSN = CONFIG_VALUE_URLSTRING( "CachalotPlugin", "DSN", "" );

        LOGGER_MESSAGE( "Cachalot DSN: %s"
            , CachalotPlugin_DSN.c_str()
        );

        CachalotLoggerPtr loggerCachalot = Helper::makeFactorableUnique<CachalotLogger>( MENGINE_DOCUMENT_FACTORABLE );

        loggerCachalot->setDSN( CachalotPlugin_DSN );

        uint32_t loggerFilter = MAKE_LOGGER_FILTER( LFILTER_PROTECTED | LFILTER_HTTP );
        loggerCachalot->setVerboseFilter( loggerFilter );

#if defined(MENGINE_MASTER_RELEASE_ENABLE)
        loggerCachalot->setVerboseLevel( LM_ERROR );
#else
        loggerCachalot->setVerboseLevel( LM_MESSAGE );
#endif

        loggerCachalot->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerCachalot ) == true )
        {
            m_loggerCachalot = loggerCachalot;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotPlugin::_finalizePlugin()
    {
        if( m_loggerCachalot != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerCachalot );

            m_loggerCachalot = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
