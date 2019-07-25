#include "PythonScriptProviderService.h"

#include "Interface/OptionsServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#if defined(MENGINE_WINDOWS_DEBUG) && !defined(MENGINE_TOOLCHAIN_MINGW)
#   include <crtdbg.h>
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ScriptProviderService, Mengine::PythonScriptProviderService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScriptProviderService::PythonScriptProviderService()
        : m_kernel( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptProviderService::~PythonScriptProviderService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptProviderService::_initializeService()
    {
#if defined(MENGINE_WINDOWS_DEBUG) && !defined(MENGINE_TOOLCHAIN_MINGW)
        int crt_warn = _CrtSetReportMode( _CRT_WARN, _CRTDBG_REPORT_MODE );
        int crt_error = _CrtSetReportMode( _CRT_ERROR, _CRTDBG_REPORT_MODE );
        int crt_assert = _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_REPORT_MODE );
#endif        

        pybind::kernel_interface * kernel = pybind::initialize( nullptr, nullptr, MENGINE_DEBUG_ATTRIBUTE( true, false ), false, true );

#if defined(MENGINE_WINDOWS_DEBUG) && !defined(MENGINE_TOOLCHAIN_MINGW)
        _CrtSetReportMode( _CRT_WARN, crt_warn );
        _CrtSetReportMode( _CRT_ERROR, crt_error );
        _CrtSetReportMode( _CRT_ASSERT, crt_assert );
#endif

        MENGINE_ASSERTION_MEMORY_PANIC( kernel, false, "invalid initialize pybind" );

        m_kernel = kernel;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptProviderService::_finalizeService()
    {
        m_kernel->destroy();
        m_kernel = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * PythonScriptProviderService::getKernel() const
    {
        return m_kernel;
    }
}