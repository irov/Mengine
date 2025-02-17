#include "PythonDocumentTraceback.h"

#include "Interface/DocumentServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/LoggerMessage.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * getStatetracePybind()
        {
            static MENGINE_THREAD_LOCAL Char statetrace[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

            kernel->get_statetrace( statetrace, MENGINE_LOGGER_MAX_MESSAGE );

            return statetrace;
        }
        //////////////////////////////////////////////////////////////////////////
        DocumentInterfacePtr createDocumentPybind( const Char * _file, uint32_t _line, const Char * _function )
        {
            Char traceback[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

            kernel->get_statetrace( traceback, MENGINE_LOGGER_MAX_MESSAGE );

            DocumentInterfacePtr doc = DOCUMENT_SERVICE()
                ->createDocument( nullptr, MENGINE_CODE_LIBRARY, _file, _line, _function, "traceback: %s", traceback );

            return doc;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}