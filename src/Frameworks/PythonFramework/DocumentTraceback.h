#pragma once

#include "Kernel/Document.h"

#include "Interface/ScriptServiceInterface.h"

namespace Mengine
{
#ifndef NDEBUG
#	define MENGINE_DOCUMENT_PYBIND []() -> const Char * { static Char traceback[4096]; pybind::kernel_interface * kernel = SCRIPT_SERVICE()->getKernel(); kernel->get_traceback(traceback, 4096); return traceback;}()
#else
#	define MENGINE_DOCUMENT_PYBIND ""
#endif
}