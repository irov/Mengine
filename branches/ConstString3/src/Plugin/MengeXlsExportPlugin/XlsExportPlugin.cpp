#	include "XlsExportPlugin.h"

#	include "Interface/LogSystemInterface.h"

#	include "Logger/Logger.h"

#	include <Windows.h>

//////////////////////////////////////////////////////////////////////////
bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
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
	void XlsExportPlugin::initialize( ServiceProviderInterface * _provider, const TMapParam & _params )
	{
		LogServiceInterface * logger_service = _provider->getServiceT<LogServiceInterface>( "Log" );

		if( logger_service == 0 )
		{
			return;
		}

		LogSystemInterface * logSystem = logger_service->getInterface();

		const char * xls2xml = "xls2xml.dll";

		HINSTANCE hMyDll = ::LoadLibraryA("xls2xml.dll");

		if( hMyDll == NULL )
		{
			LOGGER_ERROR(logSystem)( "Error: can't load dll '%s'"
				, xls2xml
				);

			return;
		}

		typedef int (*PFN_xlsxExporter)( const char * );
		PFN_xlsxExporter p_xlsxExporter = (PFN_xlsxExporter)::GetProcAddress(hMyDll, "xlsxExporter");

		TMapParam::const_iterator it_found = _params.find("ProjectName");

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