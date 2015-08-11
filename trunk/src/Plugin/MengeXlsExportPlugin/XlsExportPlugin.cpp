#	include "XlsExportPlugin.h"

#	include "Interface/LogSystemInterface.h"
#	include "Logger/Logger.h"

#	include "Interface/ApplicationInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "XlsScriptLogger.h"

#   include "WindowsLayer/WindowsIncluder.h"

#	include <pybind\pybind.hpp>

#pragma comment ( lib, "Ws2_32.lib" )
#pragma comment ( lib, "Crypt32.lib" )

//////////////////////////////////////////////////////////////////////////
extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	bool initPluginMengeXlsExport( Menge::PluginInterface ** _plugin )
	{
		stdex_allocator_initialize();

		*_plugin = new Menge::XlsExportPlugin();

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
#	ifdef MENGE_PLUGIN_DLL
	__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
	{
		return initPluginMengeXlsExport( _plugin );
	}
#	endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XlsExportPlugin::XlsExportPlugin()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool XlsExportPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
        m_serviceProvider = _serviceProvider;

		const ConstString & projectCodename = APPLICATION_SERVICE(m_serviceProvider)
            ->getProjectCodename();

        if( projectCodename.empty() == true )
        {
            return false;
        }
	
		//Py_IgnoreEnvironmentFlag = 1;
		//Py_VerboseFlag = 1;
		//Py_NoUserSiteDirectory = 1;
		//Py_NoSiteFlag = 1;

		WChar currentPath[MENGINE_MAX_PATH];
		PLATFORM_SERVICE( m_serviceProvider )
			->getCurrentPath( currentPath, MENGINE_MAX_PATH );

        {
			WString exportPath;
			exportPath += currentPath;
			exportPath += L"Python3Lib\\";

			WChar shortpath[MENGINE_MAX_PATH];
			PLATFORM_SERVICE( m_serviceProvider )
				->getShortPathName( exportPath, shortpath, MENGINE_MAX_PATH );
			
			pybind::set_path( shortpath );
        }        

		pybind::initialize( false, false, true );

        PyObject * xls_module = pybind::module_init( "Xls" );
		
        pybind::set_currentmodule( xls_module );

        XlsScriptLogger * logger = new XlsScriptLogger(m_serviceProvider);

        PyObject * pyLogger = logger->embedding();
        pybind::setStdOutHandle( pyLogger );

        XlsScriptLoggerError * errorLogger = new XlsScriptLoggerError(m_serviceProvider);

        PyObject * pyErrorLogger = errorLogger->embedding();
        pybind::setStdErrorHandle( pyErrorLogger );

        PyObject * py_syspath = pybind::list_new(0);

		{
			WString stdPath;
			stdPath += currentPath;
			stdPath += L"Python3Lib\\";

			WChar shortpath[MENGINE_MAX_PATH];
			PLATFORM_SERVICE( m_serviceProvider )
				->getShortPathName( stdPath, shortpath, MENGINE_MAX_PATH );

			pybind::list_appenditem_t( py_syspath, shortpath );			
		}

		{
			WString stdPath;
			stdPath += currentPath;
			stdPath += L"XlsxExport\\";

			WChar shortpath[MENGINE_MAX_PATH];
			PLATFORM_SERVICE( m_serviceProvider )
				->getShortPathName( stdPath, shortpath, MENGINE_MAX_PATH );

			pybind::list_appenditem_t( py_syspath, shortpath );
		}

		pybind::set_syspath( py_syspath );

		pybind::decref( py_syspath );
		
		this->proccess_( projectCodename.c_str() );

		pybind::finalize();

        delete logger;
        delete errorLogger;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::finalize()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::destroy()
	{
		delete this;

		stdex_allocator_finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	bool XlsExportPlugin::proccess_( const char * _projectName )
	{
		PyObject * module_builtins = pybind::get_builtins();

		pybind::def_functor( "Error", this, &XlsExportPlugin::error_, module_builtins );

		bool exist = false;
		PyObject * py_xlsxExporter = pybind::module_import( "xlsxExporter", exist );

		if( py_xlsxExporter == 0 )
		{
			return false;
		}

		pybind::call_method( py_xlsxExporter, "export", "(s)"
			, _projectName
			);

        pybind::decref( py_xlsxExporter );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::error_( const wchar_t * _msg )
	{
		LOGGER_ERROR(m_serviceProvider)("%ls"
			, _msg
			);
	}
}