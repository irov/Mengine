#include "Win32ConsoleLoggerPlugin.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/OptionHelper.h"

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
        bool OPTION_nologs = HAS_OPTION( "nologs" );

        if( OPTION_nologs == true )
        {
            return false;
        }

        bool OPTION_console = HAS_OPTION( "console" );

        if( OPTION_console == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ConsoleLoggerPlugin::_initializePlugin()
    {
        Win32ConsoleLoggerPtr loggerConsole = Helper::makeFactorableUnique<Win32ConsoleLogger>( MENGINE_DOCUMENT_FACTORABLE );

        loggerConsole->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerConsole ) == true )
        {
            m_loggerConsole = loggerConsole;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ConsoleLoggerPlugin::_finalizePlugin()
    {
        if( m_loggerConsole != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerConsole );

            m_loggerConsole = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
