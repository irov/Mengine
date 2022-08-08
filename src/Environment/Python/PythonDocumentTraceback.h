#pragma once

#include "Interface/DocumentServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Win32Helper.h"

//////////////////////////////////////////////////////////////////////////
#define MENGINE_PYBIND_TRACEBACK() \
[](){ \
    static MENGINE_THREAD_LOCAL Mengine::Char traceback[8192] = {'\0'}; \
    pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel(); \
    kernel->get_traceback( traceback, 8192 ); \
    return traceback; \
}()

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DOCUMENT_ENABLE
#   define MENGINE_DOCUMENT_PYBIND [](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) \
    { \
        Mengine::Char traceback[8192] = {'\0'}; \
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel(); \
        kernel->get_traceback(traceback, 8192); \
        Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( nullptr, Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "traceback: %s", traceback); \
        return doc; \
    }(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#else
#   define MENGINE_DOCUMENT_PYBIND nullptr
#endif
//////////////////////////////////////////////////////////////////////////