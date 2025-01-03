#include "PythonDocumentTraceback.h"

#include "Interface/DocumentServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#ifndef MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE
#define MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE 8192
#endif

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * getTracebackPybind()
        {
            static MENGINE_THREAD_LOCAL Char traceback[MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE + 1] = {'\0'};

            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

            kernel->get_traceback( traceback, MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE );

            return traceback;
        }
        //////////////////////////////////////////////////////////////////////////
        DocumentInterfacePtr createDocumentPybind( const Char * _file, const Char * _function, uint32_t _line )
        {
            Char traceback[MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE + 1] = {'\0'};

            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

            kernel->get_traceback( traceback, MENGINE_PYBIND_TRACEBACK_MAX_MESSAGE );

            DocumentInterfacePtr doc = DOCUMENT_SERVICE()
                ->createDocument( nullptr, MENGINE_CODE_LIBRARY, _file, _function, _line, "traceback: %s", traceback );

            return doc;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}