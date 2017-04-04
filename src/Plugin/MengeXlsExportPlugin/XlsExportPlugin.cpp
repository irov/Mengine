#	include "XlsExportPlugin.h"

#	include "Logger/Logger.h"

#	include "Interface/ApplicationInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "WIN32/WindowsIncluder.h"

#	include <pybind\pybind.hpp>

#pragma comment ( lib, "Ws2_32.lib" )
#pragma comment ( lib, "Crypt32.lib" )

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeXlsExport, Menge::XlsExportPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XlsExportPlugin::XlsExportPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool XlsExportPlugin::_initialize()
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

        currentPath[len] = L'\\';
        currentPath[len + 1] = L'\0';

        {
			WChar exportPath[MENGINE_MAX_PATH];
            wcscpy( exportPath, currentPath );
            wcscat( exportPath, L"Python3Lib\\" );

			WChar shortpath[MENGINE_MAX_PATH];
            GetShortPathName( exportPath, shortpath, MENGINE_MAX_PATH );
			
			pybind::set_path( shortpath );
        }        

		pybind::kernel_interface * kernel = pybind::initialize( false, false, true );

        //PyObject * xls_module = pybind::module_init( "Xls" );
		
        //pybind::set_currentmodule( xls_module );

		PyObject * module_builtins = pybind::get_builtins();

		m_warninglogger = new XlsScriptLogger( m_serviceProvider, LM_WARNING );

		PyObject * pyWarningLogger = m_warninglogger->embedding( kernel, module_builtins );
        pybind::setStdOutHandle( pyWarningLogger );

		m_errorLogger = new XlsScriptLogger( m_serviceProvider, LM_ERROR );

		PyObject * pyErrorLogger = m_errorLogger->embedding( kernel, module_builtins );
        pybind::setStdErrorHandle( pyErrorLogger );

        PyObject * py_syspath = pybind::list_new(0);

		{
            WChar stdPath[MENGINE_MAX_PATH];
            wcscpy( stdPath, currentPath );
            wcscat( stdPath, L"Python3Lib\\" );

            WChar shortpath[MENGINE_MAX_PATH];
            GetShortPathName( stdPath, shortpath, MENGINE_MAX_PATH );

			pybind::list_appenditem_t( kernel, py_syspath, shortpath );			
		}

		{
            WChar xlsxPath[MENGINE_MAX_PATH];
            wcscpy( xlsxPath, currentPath );
            wcscat( xlsxPath, L"XlsxExport\\" );

			WChar shortpath[MENGINE_MAX_PATH];
            GetShortPathName( xlsxPath, shortpath, MENGINE_MAX_PATH );

			pybind::list_appenditem_t( kernel, py_syspath, shortpath );
		}

		pybind::set_syspath( py_syspath );

		pybind::decref( py_syspath );
		
        pybind::def_functor( kernel, "Warning", this, &XlsExportPlugin::warning_, module_builtins );
        pybind::def_functor( kernel, "Error", this, &XlsExportPlugin::error_, module_builtins );

		m_observerChangeLocale = NOTIFICATION_SERVICE( m_serviceProvider )
			->addObserverMethod( NOTIFICATOR_CHANGE_LOCALE_PREPARE, this, &XlsExportPlugin::notifyChangeLocale );
				
		this->proccess_();

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::_finalize()
	{
		pybind::finalize();

		delete m_warninglogger;
		delete m_errorLogger;

		m_observerChangeLocale = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale )
	{
		(void)_prevLocale;
		(void)_currentlocale;

		this->proccess_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool XlsExportPlugin::proccess_()
	{
		const ConstString & projectCodename = APPLICATION_SERVICE( m_serviceProvider )
			->getProjectCodename();

		if( projectCodename.empty() == true )
		{
			return false;
		}

		bool exist = false;
		PyObject * py_xlsxExporter = pybind::module_import( "xlsxExporter", exist );

		if( py_xlsxExporter == 0 )
		{
			return false;
		}

		pybind::call_method( py_xlsxExporter, "export", "(s)"
			, projectCodename.c_str()
			);

        pybind::decref( py_xlsxExporter );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::warning_(const wchar_t * _msg)
	{
		Char utf8_msg[2048];
		UNICODE_SERVICE(m_serviceProvider)
			->unicodeToUtf8(_msg, (size_t)-1, utf8_msg, 2048, nullptr);

		LOGGER_WARNING(m_serviceProvider)("%s"
			, utf8_msg
			);

	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::error_( const wchar_t * _msg )
	{
		Char utf8_msg[2048];
		UNICODE_SERVICE( m_serviceProvider )
			->unicodeToUtf8( _msg, (size_t)-1, utf8_msg, 2048, nullptr );

		LOGGER_ERROR(m_serviceProvider)("%s"
			, utf8_msg
			);
	}
}