#include "PythonDocument.h"

#include "Interface/DocumentServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/LoggerMessage.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        DocumentInterfacePtr createPythonDocument( const Char * _file, int32_t _line, const Char * _function )
        {
            Char traceback[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

            kernel->get_statetrace( traceback, MENGINE_LOGGER_MAX_MESSAGE, false );

            DocumentInterfacePtr doc = DOCUMENT_SERVICE()
                ->createDocument( nullptr, MENGINE_CODE_LIBRARY, _file, _line, _function, "traceback: %s", traceback );

            return doc;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
