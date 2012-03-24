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
		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "LogService" );

		if( logService == 0 )
		{
			return;
		}
		
		TMapParam::const_iterator it_found = _params.find("ProjectName");

		if( it_found == _params.end() )
		{
			return;
		}

		const String & projectName = it_found->second;
		
		Py_IgnoreEnvironmentFlag = 1;
		//Py_VerboseFlag = 1;
		//Py_NoUserSiteDirectory = 1;
		Py_NoSiteFlag = 1;
		
		WString xlsxExportPath = L"d:\\Projects\\Antoinette\\Bin2\\XlsxExport\\";
		Py_SetPath(xlsxExportPath.c_str());
				
		pybind::initialize(false, false);

		std::string stdPath = "d:\\Projects\\Antoinette\\Bin2\\XlsxExport\\";

		PyObject * py_syspath = pybind::list_new(0);

		PyObject * py_stdPath = pybind::unicode_from_utf8(stdPath.c_str(), stdPath.size());

		pybind::list_appenditem( py_syspath, py_stdPath );

		pybind::decref( py_stdPath );

		pybind::set_syspath( py_syspath );

		pybind::decref( py_syspath );		

		this->proccess( projectName );

		pybind::finalize();
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