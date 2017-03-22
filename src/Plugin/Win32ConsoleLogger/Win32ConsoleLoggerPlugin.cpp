#	include "Win32ConsoleLoggerPlugin.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/OptionsInterface.h"

#   include "Win32ConsoleLogger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32ConsoleLogger, Menge::Win32ConsoleLoggerPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
    Win32ConsoleLoggerPlugin::Win32ConsoleLoggerPlugin()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool Win32ConsoleLoggerPlugin::_avaliable()
    {
        bool nologs = HAS_OPTION( m_serviceProvider, "nologs" );

        if( nologs == true )
        {
            return false;
        }

        bool console = HAS_OPTION( m_serviceProvider, "console" );

        if( console == false )
        {
            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32ConsoleLoggerPlugin::_initialize()
	{
        m_loggerConsole = new FactorableUnique<Win32ConsoleLogger>();

        LOGGER_SERVICE( m_serviceProvider )
            ->registerLogger( m_loggerConsole );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32ConsoleLoggerPlugin::_finalize()
	{
        LOGGER_SERVICE( m_serviceProvider )
            ->unregisterLogger( m_loggerConsole );
	}
}
