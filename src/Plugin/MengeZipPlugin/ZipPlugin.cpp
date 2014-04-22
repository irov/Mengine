#	include "ZipPlugin.h"

#	include "Interface/LogSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Interface/FileSystemInterface.h"

#	include "FileGroupZip.h"
#	include "ArchivatorZip.h"

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    bool initPluginMengeZip( Menge::PluginInterface ** _plugin )
    {
        *_plugin = new Menge::ZipPlugin();

        return true;
    }
#   ifdef MENGE_PLUGIN_DLL
    ////////////////////////////////////////////////////////////////////////////
    __declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
    {
        return initPluginMengeZip( _plugin );
    }
#   endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ZipPlugin::ZipPlugin()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ZipPlugin::initialize( ServiceProviderInterface * _provider )
	{
        m_serviceProvider = _provider;
	
		FILE_SERVICE(m_serviceProvider)
			->registerFileGroupFactory( Helper::stringizeString(m_serviceProvider, "zip"), new FactorableUnique<FactoryDefault<FileGroupZip> >() );

		ARCHIVE_SERVICE(m_serviceProvider)
			->registerArchivator( Helper::stringizeString(m_serviceProvider, "zip"), new FactorableUnique<ArchivatorZip>() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ZipPlugin::destroy()
	{
		FILE_SERVICE(m_serviceProvider)
			->unregisterFileGroupFactory( Helper::stringizeString(m_serviceProvider, "zip") );
		
		ARCHIVE_SERVICE(m_serviceProvider)
			->unregisterArchivator( Helper::stringizeString(m_serviceProvider, "zip") );

		delete this;
	}
}