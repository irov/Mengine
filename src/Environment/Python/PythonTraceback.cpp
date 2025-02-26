#include "PythonTraceback.h"

#include "Kernel/Stringalized.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        void getPythonTracebackMessage( Char * const _message, size_t _capacity, pybind::kernel_interface * _kernel, PyObject * _traceback )
        {
            StdString::strncpy( _message, "Traceback:", _capacity );

            if( _traceback == nullptr || _kernel->traceback_check( _traceback ) == false )
            {
                StdString::strncpy( _message, "No traceback available.", _capacity );

                return;
            }

            PyObject * tb = _traceback;

            while( tb != nullptr )
            {
                StdString::strncat( _message, "\n", _capacity );
                StdString::strncat( _message, "File: ", _capacity );
                StdString::strncat( _message, _kernel->traceback_filename( tb ).c_str(), _capacity );
                StdString::strncat( _message, ", line ", _capacity );
                uint32_t lineno = _kernel->traceback_lineno( tb );
                Char lineno_str[32 + 1] = {'\0'};
                Helper::stringalized( lineno, lineno_str, 32 );
                StdString::strncat( _message, lineno_str, _capacity );
                StdString::strncat( _message, ", in ", _capacity );
                StdString::strncat( _message, _kernel->traceback_function( tb ).c_str(), _capacity );

                tb = _kernel->traceback_next( tb );
            }
        }
    }
}