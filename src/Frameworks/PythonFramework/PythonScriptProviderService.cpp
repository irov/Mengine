#include "PythonScriptProviderService.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/AllocatorServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryAllocator.h"

#if defined(MENGINE_WINDOWS_DEBUG) && !defined(MENGINE_TOOLCHAIN_MINGW)
#   include <crtdbg.h>
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ScriptProviderService, Mengine::PythonScriptProviderService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        class MyPythonAllocator
            : public pybind::allocator_interface
        {
        public:
            MyPythonAllocator()
            {
            }

            ~MyPythonAllocator() override
            {
            }

        protected:
            void * malloc( size_t _size ) override
            {
                void * p = ALLOCATOR_SERVICE()
                    ->malloc( _size, "python" );

                return p;
            }

            void * calloc( size_t _num, size_t _size ) override
            {
                void * p = ALLOCATOR_SERVICE()
                    ->calloc( _num, _size, "python" );

                return p;
            }

            void * realloc( void * _ptr, size_t _size ) override
            {
                void * p = ALLOCATOR_SERVICE()
                    ->realloc( _ptr, _size, "python" );

                return p;
            }

            void free( void * _ptr ) override
            {
                ALLOCATOR_SERVICE()
                    ->free( _ptr, "python" );
            }
        };
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
        int32_t crt_warn = _CrtSetReportMode( _CRT_WARN, _CRTDBG_REPORT_MODE );
        int32_t crt_error = _CrtSetReportMode( _CRT_ERROR, _CRTDBG_REPORT_MODE );
        int32_t crt_assert = _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_REPORT_MODE );
#endif

        pybind::allocator_interface * allocator = Helper::newT<Detail::MyPythonAllocator>();

        pybind::kernel_interface * kernel = pybind::initialize( allocator, nullptr, MENGINE_DEBUG_VALUE( true, false ), false, true );

#if defined(MENGINE_WINDOWS_DEBUG) && !defined(MENGINE_TOOLCHAIN_MINGW)
        _CrtSetReportMode( _CRT_WARN, crt_warn );
        _CrtSetReportMode( _CRT_ERROR, crt_error );
        _CrtSetReportMode( _CRT_ASSERT, crt_assert );
#endif

        MENGINE_ASSERTION_MEMORY_PANIC( kernel, "invalid initialize pybind" );

        m_kernel = kernel;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptProviderService::_finalizeService()
    {
        pybind::allocator_interface * allocator = m_kernel->get_allocator();

        m_kernel->destroy();
        m_kernel = nullptr;

        Helper::deleteT( static_cast<Detail::MyPythonAllocator *>(allocator) );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * PythonScriptProviderService::getKernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
}