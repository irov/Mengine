#	include "XlsExportPlugin.h"
#	include "Interface/LogSystemInterface.h"
#	include "Logger/Logger.h"
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
	void XlsExportPlugin::initialize( ServiceProviderInterface * _provider, const TMapParam & _params )
	{
		
		//pybind::initialize(false);
		//
		//wchar_t* home = Py_GetPythonHome();
		//wchar_t * path = Py_GetPath();
		//Py_SetPythonHome(L"C:\\Python32");
		
		Py_IgnoreEnvironmentFlag = 1;
		//Py_VerboseFlag = 1;
		//Py_NoUserSiteDirectory = 1;
		Py_NoSiteFlag = 1;

		Py_SetPath(L"d:\\Projects\\Antoinette\\Bin2\\XlsxExport\\");

		Py_Initialize();

		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "LogService" );

		if( logService == 0 )
		{
			return;
		}


		//
		//Py_Initialize();



		TMapParam::const_iterator it_found = _params.find("ProjectName");

		if( it_found != _params.end() )
		{
			const std::string & path = it_found->second;
			PyRun_SimpleString("from xlsxExporter import export\n");
			char buffer[255] = {'\0'};
			sprintf( buffer, "export(\"%s\")", path.c_str() );
			PyRun_SimpleString( buffer );
		}

		Py_Finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::finalize()
	{
		delete this;
	}
}