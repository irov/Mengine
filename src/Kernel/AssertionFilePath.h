#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/FilePath.h"

#   include "Config/Char.h"
#   include "Config/Typedef.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionValidateFilePath( const Char * _category, const FilePath & _filePath, const Char * _file, int32_t _line, const Char * _function );
    }
}

#   define MENGINE_ASSERTION_VALIDATE_FILEPATH(Path) Detail::assertionValidateFilePath(MENGINE_CODE_LIBRARY, Path, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
#else
#   define MENGINE_ASSERTION_VALIDATE_FILEPATH(Path)
#endif
