#include "CachalotPlugin.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/OptionHelper.h"

#include "CachalotLogger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Cachalot, Mengine::CachalotPlugin )
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CachalotPlugin::_initializePlugin()
    {
        CachalotLoggerPtr loggerCachalot = Helper::makeFactorableUnique<CachalotLogger>( MENGINE_DOCUMENT_FACTORABLE );

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
