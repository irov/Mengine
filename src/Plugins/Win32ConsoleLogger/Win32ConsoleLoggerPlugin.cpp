#include "Win32ConsoleLoggerPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

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
        m_loggerConsole = Helper::makeFactorableUnique<Win32ConsoleLogger>();

        LOGGER_SERVICE()
            ->registerLogger( m_loggerConsole );

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
