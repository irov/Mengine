#   include "SDLOptionsSystem.h"

#   include "WindowsLayer/WindowsIncluder.h"


//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( OptionsSystem, Menge::SDLOptionsSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    void SDLOptionsSystem::setArgs( char ** argv, const int argc )
    {
        for (int i = 0; i < argc; ++i)
        {
            m_commandLine += argv[i];
            if (i < (argc - 1))
            {
                m_commandLine += " ";
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    bool SDLOptionsSystem::getOptions( Char * _options, size_t _capacity ) const
    {
        if( m_commandLine.size() >= _capacity )
        {
            return false;
        }

        strcpy( _options, m_commandLine.c_str() );

        return true;
    }
}
