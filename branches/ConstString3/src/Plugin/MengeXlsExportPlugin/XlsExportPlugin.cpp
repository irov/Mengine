#	include "XlsExportPlugin.h"

#	include "Interface/LogSystemInterface.h"
#	include "Logger/Logger.h"

#	include "Interface/ApplicationInterface.h"
#	include "Interface/UnicodeInterface.h"

#	include <Windows.h>
#	include "Python.h"
#	include <pybind\pybind.hpp>

//////////////////////////////////////////////////////////////////////////
bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
{
	*_plugin = new Menge::XlsExportPlugin();
	return true;
}

extern int Py_OptimizeFlag;
extern int Py_NoSiteFlag;
extern int Py_IgnoreEnvironmentFlag;
extern int Py_VerboseFlag;
extern int Py_NoUserSiteDirectory;

extern "C"
{
	extern PyObject* PyInit_pyexpat(void);
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XlsExportPlugin::XlsExportPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool XlsExportPlugin::initialize( ServiceProviderInterface * _provider )
	{
		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "LogService" );

		if( logService == 0 )
		{
			return false;
		}

		ApplicationInterface * applicationService = _provider->getServiceT<ApplicationInterface>( "ApplicationService" );

        if( applicationService == 0 )
        {
            return false;
        }

		const String & projectCodename = applicationService->getProjectCodename();

        if( projectCodename.empty() == true )
        {
            return false;
        }
	
		PlatformInterface * platform = applicationService->getPlatform();

		Py_IgnoreEnvironmentFlag = 1;
		//Py_VerboseFlag = 1;
		//Py_NoUserSiteDirectory = 1;
		Py_NoSiteFlag = 1;

		const WString & currentPath = platform->getCurrentPath();
		
		WString xlsxExportPath = currentPath + L"\\XlsxExport\\";
		
		Py_SetPath( xlsxExportPath.c_str() );

		pybind::initialize( false, false );

		WString stdPath = currentPath + L"\\XlsxExport\\";

		PyObject * py_syspath = pybind::list_new(0);

		PyObject * py_stdPath = pybind::unicode_from_wchar_size(stdPath.c_str(), stdPath.size());

		pybind::list_appenditem( py_syspath, py_stdPath );

		pybind::decref( py_stdPath );

		pybind::set_syspath( py_syspath );

		pybind::decref( py_syspath );		

		this->proccess( projectCodename );

		pybind::finalize();

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::finalize()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XlsExportPlugin::proccess( const String & _projectName )
	{
		bool exist = false;
		PyObject * py_xlsxExporter = pybind::module_import( "xlsxExporter", exist );

		if( py_xlsxExporter == 0 )
		{
			return false;
		}

		pybind::call_method( py_xlsxExporter, "export", "(s)"
			, _projectName.c_str()
			);

		return true;
	}
}