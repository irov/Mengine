#pragma once

#include "Kernel/Document.h"

#include "Interface/ScriptProviderServiceInterface.h"

namespace Mengine
{
#ifdef MENGINE_DEBUG
#	define MENGINE_DOCUMENT_PYBIND []() -> const Char * \
    { \
        static Char buffer[5120]; \
        static Char traceback[4096]; \
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel(); \
        kernel->get_traceback(traceback, 8192); \
        snprintf(buffer, 5120, "%s\nfile %s:%d\ncall: %s", traceback, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION); \
        return buffer;}()
#else
#	define MENGINE_DOCUMENT_PYBIND ""
#endif
}