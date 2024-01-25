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

        bool OPTION_cachalot = HAS_OPTION( "cachalot" );

        if( OPTION_cachalot == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CachalotPlugin::_initializePlugin()
    {
        CachalotLoggerPtr loggerCachalot = Helper::makeFactorableUnique<CachalotLogger>( MENGINE_DOCUMENT_FACTORABLE );

        loggerCachalot->setWriteHistory( true );

        LOGGER_SERVICE()
            ->registerLogger( loggerCachalot );

        m_loggerCachalot = loggerCachalot;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotPlugin::_finalizePlugin()
    {
        LOGGER_SERVICE()
            ->unregisterLogger( m_loggerCachalot );

        m_loggerCachalot = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
