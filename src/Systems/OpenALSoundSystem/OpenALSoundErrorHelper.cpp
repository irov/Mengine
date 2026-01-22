#include "OpenALSoundErrorHelper.h"

#if defined(MENGINE_OPENAL_RENDER_CHECK_ERROR_ENABLE)
//////////////////////////////////////////////////////////////////////////
#   include "Kernel/Logger.h"
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        bool OpenALErrorCheck( ALenum _err, const Char * _method, const Char * _file, int32_t _line, const Char * _function, const String & _args )
        {
            const Char * message = alGetString( _err );

            LOGGER_VERBOSE_LEVEL( "openal", LM_ERROR, LFILTER_NONE, LCOLOR_RED, _file, _line, _function, LFLAG_FULL )("call '%s(%s)' error %s [%d]"
                , _method
                , _args.c_str()
                , message != nullptr ? message : "unknown"
                , _err
                );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#endif
