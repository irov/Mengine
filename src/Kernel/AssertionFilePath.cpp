#include "AssertionFilePath.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Assertion.h"

#   include "Config/StdIO.h"
#   include "Config/StdString.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionValidateFilePath( const Char * _category, const FilePath & _filePath, const Char * _file, uint32_t _line )
        {
            const Char * filePath_str = _filePath.c_str();

            if( MENGINE_STRCHR( filePath_str, ' ' ) != nullptr )
            {
                Char msg[MENGINE_ASSERTION_MAX_MESSAGE] = {'\0'};
                MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "filepath '%s' should not contain spaces"
                    , filePath_str
                );

                Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion File Path" );
            }
        }
    }
}
#endif