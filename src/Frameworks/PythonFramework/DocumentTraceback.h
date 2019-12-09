#pragma once

#include "Kernel/Document.h"
#include "Kernel/FactorableUnique.h"

#include "Interface/ScriptProviderServiceInterface.h"

namespace Mengine
{
#ifdef MENGINE_DEBUG
#   define MENGINE_DOCUMENT_PYBIND [](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) \
    { \
        Mengine::DocumentPtr doc = Mengine::Helper::makeFactorableUnique<Mengine::Document>(_file, _function, _line); \
        Char traceback[8192]; \
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel(); \
        kernel->get_traceback(traceback, 8192); \
        doc->message("traceback: %s", traceback); \
        return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#else
#   define MENGINE_DOCUMENT_PYBIND nullptr
#endif
}