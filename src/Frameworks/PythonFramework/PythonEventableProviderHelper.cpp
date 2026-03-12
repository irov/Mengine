#include "PythonEventableProviderHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool initializePythonEventableType( pybind::kernel_interface * _kernel, const pybind::object & _type )
        {
            if( _type.is_invalid() == true || _type.is_none() == true )
            {
                return false;
            }

            if( _type.is_type_class() == true )
            {
                PyObject * py_type_ptr = _type.ptr();

                if( _kernel->type_initialize( py_type_ptr ) == false )
                {
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}