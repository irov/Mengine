#	include "XlsExportPlugin.h"

#	include <Windows.h>

//////////////////////////////////////////////////////////////////////////
bool initPluginMengeXlsExport( Menge::PluginInterface ** _plugin )
{
	*_plugin = new Menge::XlsExportPlugin();
	return true;
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XlsExportPlugin::XlsExportPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::initialize( ServiceProviderInterface * _provider )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::run( const TMapParam & _params )
	{
		HINSTANCE hMyDll = ::LoadLibraryA("xls2xml.dll");

		typedef int (*PFN_xlsxExporter)( const char * );
		PFN_xlsxExporter p_xlsxExporter = (PFN_xlsxExporter)::GetProcAddress(hMyDll, "xlsxExporter");

		TMapParam::const_iterator it_found = _params.find("Path");

		if( it_found != _params.end() )
		{
			const std::string & path = it_found->second;

			p_xlsxExporter( path.c_str() );
		}

		::FreeLibrary( hMyDll );
	}
	//////////////////////////////////////////////////////////////////////////
	void XlsExportPlugin::finalize()
	{
		delete this;
	}
}