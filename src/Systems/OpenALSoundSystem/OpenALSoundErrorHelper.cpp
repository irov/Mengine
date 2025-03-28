#include "OpenALSoundErrorHelper.h"

#include "OpenALSoundIncluder.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool OpenALErrorCheck( const Char * _method, const Char * _file, int32_t _line, const Char * _function )
        {
            ALenum error = alGetError();

            if( error == AL_NO_ERROR )
            {
                return true;
            }

            const Char * message = alGetString( error );

            LOGGER_VERBOSE_LEVEL( "openal", LM_ERROR, LFILTER_NONE, LCOLOR_RED, _file, _line, _function, LFLAG_FULL )("method '%s' error %s [%d]"
                , _method
                , message != nullptr ? message : "unknown"
                , error
                );

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
