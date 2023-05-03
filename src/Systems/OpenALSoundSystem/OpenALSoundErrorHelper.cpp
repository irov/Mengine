#include "OpenALSoundErrorHelper.h"

#include "OpenALSoundIncluder.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool OpenALErrorCheck( const Char * _method, const Char * _file, uint32_t _line )
        {
            ALenum error = alGetError();

            if( error == AL_NO_ERROR )
            {
                return true;
            }

            const Char * message = alGetString( error );

            LOGGER_VERBOSE_LEVEL( "openal", LM_ERROR, LFILTER_NONE, LCOLOR_RED, nullptr, 0, LFLAG_SHORT )("OpenAL Error: method '%s' (%s[%u]) %d:%s"
                , _method
                , _file
                , _line
                , error
                , message != nullptr ? message : "unknown"
                );

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
