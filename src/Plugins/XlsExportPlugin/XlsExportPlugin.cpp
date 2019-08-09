#include "XlsExportPlugin.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func( void )
{
    return _iob;
}
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( XlsExport, Mengine::XlsExportPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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
    bool XlsExportPlugin::_initializePlugin()
    {
        //Py_IgnoreEnvironmentFlag = 1;
        //Py_VerboseFlag = 1;
        //Py_NoUserSiteDirectory = 1;
        //Py_NoSiteFlag = 1;

        WChar currentPath[MENGINE_MAX_PATH];
        DWORD len = (DWORD)::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

        if( len == 0 )
        {
            return false;
        }

        currentPath[len] = L'/';
        currentPath[len + 1] = L'\0';

        WChar exportPath[MENGINE_MAX_PATH];
        wcscpy( exportPath, currentPath );
        wcscat( exportPath, L"Python3Lib/" );

        WChar shortpath_exportPath[MENGINE_MAX_PATH];
        DWORD ShortPathNameLen = ::GetShortPathName( exportPath, shortpath_exportPath, MENGINE_MAX_PATH );

        if( ShortPathNameLen == 0 )
        {
            return false;
        }

        pybind::kernel_interface * kernel = pybind::initialize( nullptr, shortpath_exportPath, false, false, true );

        //PyObject * xls_module = pybind::module_init( "Xls" );

        //pybind::set_currentmodule( xls_module );

        PyObject * module_builtins = kernel->get_builtins();

        pybind::interface_<XlsScriptLogger>( kernel, "XlsScriptLogger", true, module_builtins )
            .def_native_kernel( "write", &XlsScriptLogger::py_write )
            .def_native_kernel( "flush", &XlsScriptLogger::py_flush )
            .def_property( "softspace", &XlsScriptLogger::getSoftspace, &XlsScriptLogger::setSoftspace )
            ;

        m_warninglogger = new XlsScriptLogger( LM_WARNING );

        PyObject * pyWarningLogger = pybind::ptr( kernel, m_warninglogger );
        kernel->setStdOutHandle( pyWarningLogger );

        m_errorLogger = new XlsScriptLogger( LM_ERROR );

        PyObject * pyErrorLogger = pybind::ptr( kernel, m_warninglogger );
        kernel->setStdErrorHandle( pyErrorLogger );

        pybind::list py_syspath( kernel );

        {
            WChar stdPath[MENGINE_MAX_PATH];
            wcscpy( stdPath, currentPath );
            wcscat( stdPath, L"Python3Lib/" );

            WChar shortpath_stdPath[MENGINE_MAX_PATH];
            GetShortPathName( stdPath, shortpath_stdPath, MENGINE_MAX_PATH );

            py_syspath.append( shortpath_stdPath );
        }

        {
            WChar xlsxPath[MENGINE_MAX_PATH];
            wcscpy( xlsxPath, currentPath );
            wcscat( xlsxPath, L"XlsxExport/" );

            WChar shortpath_xlsxPath[MENGINE_MAX_PATH];
            GetShortPathName( xlsxPath, shortpath_xlsxPath, MENGINE_MAX_PATH );

            py_syspath.append( shortpath_xlsxPath );
        }

        kernel->set_syspath( py_syspath.ptr() );

        pybind::def_functor( kernel, "Warning", this, &XlsExportPlugin::warning_, module_builtins );
        pybind::def_functor( kernel, "Error", this, &XlsExportPlugin::error_, module_builtins );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_LOCALE_PREPARE, this, &XlsExportPlugin::notifyChangeLocale );

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

        kernel->destroy();

        delete m_warninglogger;
        delete m_errorLogger;

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE_PREPARE );
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale )
    {
        MENGINE_UNUSED( _prevLocale );
        MENGINE_UNUSED( _currentlocale );

        this->proccess_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::proccess_()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        ConstString projectCodename = CONFIG_VALUE( "Project", "Codename", ConstString::none() );

        if( projectCodename.empty() == true )
        {
            return false;
        }

        bool exist = false;
        PyObject * py_xlsxExporter = kernel->module_import( "xlsxExporter", exist );

        if( py_xlsxExporter == 0 )
        {
            return false;
        }

        kernel->call_method( py_xlsxExporter, "export", "(s)"
            , projectCodename.c_str()
        );

        kernel->decref( py_xlsxExporter );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::warning_( const wchar_t * _msg )
    {
        Char utf8_msg[2048];
        Helper::unicodeToUtf8( _msg, utf8_msg, 2048 );

        LOGGER_WARNING( "%s"
            , utf8_msg
        );

    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::error_( const wchar_t * _msg )
    {
        Char utf8_msg[2048];
        Helper::unicodeToUtf8( _msg, utf8_msg, 2048 );

        LOGGER_ERROR( "%s"
            , utf8_msg
        );
    }
}