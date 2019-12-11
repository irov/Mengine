#pragma once

#include "Interface/DocumentServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "Kernel/Win32Helper.h"

namespace Mengine
{
#ifdef MENGINE_DEBUG
#   define MENGINE_DOCUMENT_PYBIND [](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) \
    { \
        Mengine::Char traceback[8192]; \
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel(); \
        kernel->get_traceback(traceback, 8192); \
        Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( nullptr, Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "traceback: %s", traceback); \
        return doc; \
    }(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#else
#   define MENGINE_DOCUMENT_PYBIND nullptr
#endif
}