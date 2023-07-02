#pragma once

#include "Interface/DocumentServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#ifndef MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE
#define MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE 8192
#endif

//////////////////////////////////////////////////////////////////////////
#define MENGINE_PYBIND_TRACEBACK() [](){ \
    static MENGINE_THREAD_LOCAL Mengine::Char traceback[MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE] = {'\0'}; \
    pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel(); \
    kernel->get_traceback( traceback, MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE ); \
    return traceback; \
}()

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DOCUMENT_ENABLE)
#   define MENGINE_DOCUMENT_PYBIND [](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { \
        Mengine::Char traceback[MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE] = {'\0'}; \
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel(); \
        kernel->get_traceback(traceback, MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE); \
        Mengine::DocumentInterfacePtr doc = DOCUMENT_SERVICE()->createDocument( nullptr, MENGINE_CODE_LIBRARY, _file, _function, _line, "traceback: %s", traceback); \
        return doc; \
    }(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#else
#   define MENGINE_DOCUMENT_PYBIND nullptr
#endif
//////////////////////////////////////////////////////////////////////////