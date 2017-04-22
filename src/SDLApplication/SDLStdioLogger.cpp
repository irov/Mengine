#	include "SDLStdioLogger.h"

#	include "SDL_log.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	SDLStdioLogger::SDLStdioLogger()
        : m_verboseLevel( LM_INFO )
        , m_verboseFlag( 0xFFFFFFFF )
    {
    }
    //////////////////////////////////////////////////////////////////////////
	SDLStdioLogger::~SDLStdioLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLStdioLogger::initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::setVerboseLevel( EMessageLevel _level )
    {
        m_verboseLevel = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLStdioLogger::validMessage( EMessageLevel _level, uint32_t _flag ) const
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
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count )
    {
		(void)_level;
        (void)_flag;
		(void)_count;

		SDL_Log( "%s", _data );
		//OutputDebugString( _data );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::flush()
    {
    }
}	// namespace Menge
