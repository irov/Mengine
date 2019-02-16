#include "OpenALSoundError.h"

#include "AL/al.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        bool OpenALErrorCheck( const Char * _method, const Char * _file, uint32_t _line )
        {
            ALenum error = alGetError();

            if( error == AL_NO_ERROR )
            {
                return true;
            }

            const Char * message = alGetString( error );

            LOGGER_ERROR( "OpenAL Error: method '%s' (%s[%d]) %d:%s"
                , _method
                , _file
                , _line
                , error
                , message
            );

            return false;
        }
    }
}
