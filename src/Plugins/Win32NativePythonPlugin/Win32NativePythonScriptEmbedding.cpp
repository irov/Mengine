#include "Win32NativePythonScriptEmbedding.h"

#include "Interface/Win32KernelServiceInterface.h"

#include "Win32NativePythonSemaphoreListener.h"

#include "Environment/Python/PythonDocument.h"
#include "Environment/Python/PythonIncluder.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void win32NativePythonActivateSemaphore( const ConstString & _name, const ParamVariant & _value )
        {
            WIN32_KERNEL_SERVICE()
                ->activateSemaphore( _name, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static void win32NativePythonWaitSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
        {
            SemaphoreListenerInterfacePtr listener = Helper::makeFactorableUnique<Win32NativePythonSemaphoreListener>( MENGINE_DOCUMENT_PYTHON, _cb, _args );

            WIN32_KERNEL_SERVICE()
                ->waitSemaphore( _name, listener );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32NativePythonScriptEmbedding::Win32NativePythonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32NativePythonScriptEmbedding::~Win32NativePythonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32NativePythonScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "activateSemaphore", &Detail::win32NativePythonActivateSemaphore );
        pybind::def_function_args( _kernel, "waitSemaphore", &Detail::win32NativePythonWaitSemaphore );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32NativePythonScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        WIN32_KERNEL_SERVICE()
            ->clearSemaphores();

        _kernel->remove_from_module( "activateSemaphore", nullptr );
        _kernel->remove_from_module( "waitSemaphore", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
