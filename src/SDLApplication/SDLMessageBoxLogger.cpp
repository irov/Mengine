#   include "SDLMessageBoxLogger.h"

#   include "SDL_messagebox.h"

#   include <cstdio>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLMessageBoxLogger::SDLMessageBoxLogger()
        : m_verboseLevel(LM_INFO)
        , m_verboseFlag(0xFFFFFFFF)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLMessageBoxLogger::~SDLMessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMessageBoxLogger::initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLMessageBoxLogger::finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLMessageBoxLogger::log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _size )
    {
        (void)_level;
        (void)_flag;
        (void)_size;
        
        printf( "%s", _data );
        
        if( _level == LM_CRITICAL )
        {
            SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine critical error", _data, NULL );
        }
        else if( _level == LM_FATAL )
        {
            SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine fatal error", _data, NULL );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLMessageBoxLogger::flush()
    {
        //fflush(stdout);
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLMessageBoxLogger::setVerboseLevel( EMessageLevel _level )
    {
        m_verboseLevel = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLMessageBoxLogger::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLMessageBoxLogger::validMessage( EMessageLevel _level, uint32_t _flag ) const
    {
        if( m_verboseLevel < _level )
        {
            return false;
        }

        if( _flag == 0 )
        {
            return true;
        }

        if( (m_verboseFlag & _flag) == 0 )
        {
            return false;
        }

        return true;
    }
}
