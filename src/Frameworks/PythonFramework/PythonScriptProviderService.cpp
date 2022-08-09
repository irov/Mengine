#include "PythonScriptProviderService.h"

#include "Interface/AllocatorSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

#if defined(MENGINE_WINDOWS_DEBUG) && !defined(MENGINE_TOOLCHAIN_MINGW)
#   include <crtdbg.h>
#endif

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    void * _python_malloc( size_t _size )
    {
        void * p = ALLOCATOR_SYSTEM()
            ->malloc( _size, "python" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * _python_realloc( void * _ptr, size_t _size )
    {
        void * p = ALLOCATOR_SYSTEM()
            ->realloc( _ptr, _size, "python" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void _python_free( void * _ptr )
    {
        ALLOCATOR_SYSTEM()
            ->free( _ptr, "python" );
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ScriptProviderService, Mengine::PythonScriptProviderService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        class PybindAllocator
            : public pybind::allocator_interface
        {
        public:
            PybindAllocator()
            {
            }

            ~PybindAllocator() override
            {
            }

        protected:
            void * malloc( size_t _size ) override
            {
                void * p = ALLOCATOR_SYSTEM()
                    ->malloc( _size, "pybind" );

                return p;
            }

            void * calloc( size_t _num, size_t _size ) override
            {
                void * p = ALLOCATOR_SYSTEM()
                    ->calloc( _num, _size, "pybind" );

                return p;
            }

            void * realloc( void * _ptr, size_t _size ) override
            {
                void * p = ALLOCATOR_SYSTEM()
                    ->realloc( _ptr, _size, "pybind" );

                return p;
            }

            void free( void * _ptr ) override
            {
                ALLOCATOR_SYSTEM()
                    ->free( _ptr, "pybind" );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static bool getPybindDebugMode()
        {
            bool OPTION_pybinddebug = HAS_OPTION( "pybinddebug" );

            if( OPTION_pybinddebug == true )
            {
                return true;
            }

            bool Debug_Pybind = CONFIG_VALUE( "Pybind", "Debug", true );

            return Debug_Pybind;
        }
        //////////////////////////////////////////////////////////////////////////
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

        pybind::allocator_interface * allocator = Helper::newT<Detail::PybindAllocator>();

        bool debugMode = Detail::getPybindDebugMode();

        pybind::kernel_interface * kernel = pybind::initialize( allocator, nullptr, debugMode, false, true );

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

        Helper::deleteT( static_cast<Detail::PybindAllocator *>(allocator) );

        MENGINE_ASSERTION_ALLOCATOR( "pybind" );
        MENGINE_ASSERTION_ALLOCATOR( "python" );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * PythonScriptProviderService::getKernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
}
