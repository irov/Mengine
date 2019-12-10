#include "Win32ConsoleLoggerPlugin.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"

#include "Win32ConsoleLogger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32ConsoleLogger, Mengine::Win32ConsoleLoggerPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ConsoleLoggerPlugin::Win32ConsoleLoggerPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ConsoleLoggerPlugin::~Win32ConsoleLoggerPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ConsoleLoggerPlugin::_availablePlugin() const
    {
        bool nologs = HAS_OPTION( "nologs" );

        if( nologs == true )
        {
            return false;
        }

        bool console = HAS_OPTION( "console" );

        if( console == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ConsoleLoggerPlugin::_initializePlugin()
    {
        Win32ConsoleLoggerPtr loggerConsole = Helper::makeFactorableUnique<Win32ConsoleLogger>( MENGINE_DOCUMENT_FACTORABLE );

        LOGGER_SERVICE()
            ->registerLogger( loggerConsole );

        m_loggerConsole = loggerConsole;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLoggerPlugin::_finalizePlugin()
    {
        LOGGER_SERVICE()
            ->unregisterLogger( m_loggerConsole );

        m_loggerConsole = nullptr;
    }
}
