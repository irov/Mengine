#include "PythonScriptProviderService.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/AllocatorServiceInterface.h"

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
    static void * s_python_malloc( void * _ctx, size_t _size )
    {
        MENGINE_UNUSED( _ctx );

        void * p = ALLOCATOR_SERVICE()
            ->malloc( _size, "python" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void * s_python_calloc( void * _ctx, size_t _num, size_t _size )
    {
        MENGINE_UNUSED( _ctx );

        void * p = ALLOCATOR_SERVICE()
            ->calloc( _num, _size, "python" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void * s_python_realloc( void * _ctx, void * _ptr, size_t _size )
    {
        MENGINE_UNUSED( _ctx );

        void * p = ALLOCATOR_SERVICE()
            ->realloc( _ptr, _size, "python" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_python_free( void * _ctx, void * _ptr )
    {
        MENGINE_UNUSED( _ctx );

        ALLOCATOR_SERVICE()
            ->free( _ptr, "python" );
    }
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

        pybind::kernel_domain_allocator_t domain;
        pybind::kernel_allocator_t raw;

        raw.ctx = nullptr;
        raw.malloc = &s_python_malloc;
        raw.calloc = &s_python_calloc;
        raw.realloc = &s_python_realloc;
        raw.free = &s_python_free;

        domain.raw = &raw;

        pybind::kernel_interface * kernel = pybind::initialize( &domain, nullptr, MENGINE_DEBUG_ATTRIBUTE( true, false ), false, true );

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