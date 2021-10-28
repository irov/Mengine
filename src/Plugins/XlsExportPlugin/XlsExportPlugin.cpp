#include "XlsExportPlugin.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/AllocatorServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#define MENGINE_WINDOWS_VERSION_WIN8
#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

#include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
FILE _iob[] = {*stdin, *stdout, *stderr};
extern "C" FILE * __cdecl __iob_func( void )
{
    return _iob;
}
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( XlsExport, Mengine::XlsExportPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        class MyXlsAllocator
            : public pybind::allocator_interface
        {
        public:
            MyXlsAllocator()
            {
            }

            ~MyXlsAllocator() override
            {
            }

        protected:
            void * malloc( size_t _size ) override
            {
                void * p = ALLOCATOR_SERVICE()
                    ->malloc( _size, "python3" );

                return p;
            }

            void * calloc( size_t _num, size_t _size ) override
            {
                void * p = ALLOCATOR_SERVICE()
                    ->calloc( _num, _size, "python3" );

                return p;
            }

            void * realloc( void * _ptr, size_t _size ) override
            {
                void * p = ALLOCATOR_SERVICE()
                    ->realloc( _ptr, _size, "python3" );

                return p;
            }

            void free( void * _ptr ) override
            {
                ALLOCATOR_SERVICE()
                    ->free( _ptr, "python3" );
            }
        };
    }
    //////////////////////////////////////////////////////////////////////////
    XlsExportPlugin::XlsExportPlugin()
        : m_warninglogger( nullptr )
        , m_errorLogger( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    XlsExportPlugin::~XlsExportPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "noxlsexport" ) == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::_initializePlugin()
    {
        //Py_IgnoreEnvironmentFlag = 1;
        //Py_VerboseFlag = 1;
        //Py_NoUserSiteDirectory = 1;
        //Py_NoSiteFlag = 1;

        WChar currentPath[MENGINE_MAX_PATH] = {L'\0'};
        DWORD len = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

        if( len == 0 )
        {
            return false;
        }

        currentPath[len + 0] = MENGINE_PATH_WDELIM;
        currentPath[len + 1] = L'\0';

        WChar exportPath[MENGINE_MAX_PATH] = {L'\0'};
        MENGINE_WCSCPY( exportPath, currentPath );
        MENGINE_WCSCAT( exportPath, L"Python3Lib/" );

        WChar shortpath_exportPath[MENGINE_MAX_PATH] = {L'\0'};
        DWORD ShortPathNameLen = ::GetShortPathName( exportPath, shortpath_exportPath, MENGINE_MAX_PATH );

        if( ShortPathNameLen == 0 )
        {
            return false;
        }

        Detail::MyXlsAllocator * allocator = Helper::newT<Detail::MyXlsAllocator>();

#if defined(MENGINE_WINDOWS_DEBUG) && !defined(MENGINE_TOOLCHAIN_MINGW)
        int32_t crt_warn = _CrtSetReportMode( _CRT_WARN, _CRTDBG_REPORT_MODE );
        int32_t crt_error = _CrtSetReportMode( _CRT_ERROR, _CRTDBG_REPORT_MODE );
        int32_t crt_assert = _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_REPORT_MODE );
#endif

        pybind::kernel_interface * kernel = pybind::initialize( allocator, shortpath_exportPath, false, false, true );

#if defined(MENGINE_WINDOWS_DEBUG) && !defined(MENGINE_TOOLCHAIN_MINGW)
        _CrtSetReportMode( _CRT_WARN, crt_warn );
        _CrtSetReportMode( _CRT_ERROR, crt_error );
        _CrtSetReportMode( _CRT_ASSERT, crt_assert );
#endif

        //PyObject * xls_module = pybind::module_init( "Xls" );

        //pybind::set_currentmodule( xls_module );

        PyObject * module_builtins = kernel->get_builtins();

        pybind::interface_<XlsScriptLogger>( kernel, "XlsScriptLogger", true, module_builtins )
            .def_native_kernel( "write", &XlsScriptLogger::py_write )
            .def_native_kernel( "flush", &XlsScriptLogger::py_flush )
            .def_property( "softspace", &XlsScriptLogger::getSoftspace, &XlsScriptLogger::setSoftspace )
            ;

        m_warninglogger = Helper::newT<XlsScriptLogger>( LM_WARNING, LCOLOR_RED | LCOLOR_GREEN );

        PyObject * pyWarningLogger = pybind::ptr( kernel, m_warninglogger );
        kernel->setStdOutHandle( pyWarningLogger );

        m_errorLogger = Helper::newT<XlsScriptLogger>( LM_ERROR, LCOLOR_RED );

        PyObject * pyErrorLogger = pybind::ptr( kernel, m_warninglogger );
        kernel->setStdErrorHandle( pyErrorLogger );

        pybind::list py_syspath( kernel );

        {
            WChar stdPath[MENGINE_MAX_PATH] = {L'\0'};
            MENGINE_WCSCPY( stdPath, currentPath );
            MENGINE_WCSCAT( stdPath, L"Python3Lib/" );

            WChar shortpath_stdPath[MENGINE_MAX_PATH] = {L'\0'};
            ::GetShortPathName( stdPath, shortpath_stdPath, MENGINE_MAX_PATH );

            py_syspath.append( shortpath_stdPath );
        }

        {
            WChar xlsxPath[MENGINE_MAX_PATH] = {L'\0'};
            MENGINE_WCSCPY( xlsxPath, currentPath );
            MENGINE_WCSCAT( xlsxPath, L"XlsxExport/" );

            WChar shortpath_xlsxPath[MENGINE_MAX_PATH] = {L'\0'};
            ::GetShortPathName( xlsxPath, shortpath_xlsxPath, MENGINE_MAX_PATH );

            py_syspath.append( shortpath_xlsxPath );
        }

        kernel->set_syspath( py_syspath.ptr() );

        pybind::def_functor( kernel, "Warning", this, &XlsExportPlugin::warning_, module_builtins );
        pybind::def_functor( kernel, "Error", this, &XlsExportPlugin::error_, module_builtins );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RELOAD_LOCALE, &XlsExportPlugin::notifyReloadLocale, MENGINE_DOCUMENT_FACTORABLE );

        this->proccess_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::_finalizePlugin()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        kernel->setStdOutHandle( nullptr );
        kernel->setStdErrorHandle( nullptr );

        kernel->remove_scope<XlsScriptLogger>();

        pybind::allocator_interface * allocator = kernel->get_allocator();

        kernel->destroy();

        Helper::deleteT( static_cast<Detail::MyXlsAllocator *>(allocator) );

        Helper::deleteT( m_warninglogger );
        Helper::deleteT( m_errorLogger );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RELOAD_LOCALE );
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::notifyReloadLocale()
    {
        this->proccess_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::proccess_()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        ConstString Project_Codename = CONFIG_VALUE( "Project", "Codename", ConstString::none() );

        if( Project_Codename.empty() == true )
        {
            return false;
        }

        bool exist = false;
        PyObject * py_xlsxExporter = kernel->module_import( "xlsxExporter", exist );

        if( py_xlsxExporter == nullptr )
        {
            return false;
        }

        kernel->call_method( py_xlsxExporter, "export", "(s)"
            , Project_Codename.c_str()
        );

        kernel->decref( py_xlsxExporter );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::warning_( const WChar * _msg )
    {
        Char utf8_msg[2048] = {'\0'};
        Helper::unicodeToUtf8( _msg, utf8_msg, 2047 );

        LOGGER_MESSAGE( "XlsExport[Warning]: %s"
            , utf8_msg
        );

    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::error_( const WChar * _msg )
    {
        Char utf8_msg[2048] = {'\0'};
        Helper::unicodeToUtf8( _msg, utf8_msg, 2047 );

        LOGGER_MESSAGE_ERROR( "XlsExport[Error]: %s"
            , utf8_msg
        );
    }
    //////////////////////////////////////////////////////////////////////////
}