#	include "ZipPlugin.h"

#   include "Interface/StringizeInterface.h"

#	include "Interface/FileSystemInterface.h"

#	include "FileGroupZip.h"
#	include "ArchivatorZip.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeZip, Menge::ZipPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ZipPlugin::ZipPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ZipPlugin::_initialize()
	{
		FILE_SERVICE(m_serviceProvider)
			->registerFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "zip"), new FactorableUnique<FactoryDefault<FileGroupZip> >() );
		
		ARCHIVE_SERVICE(m_serviceProvider)
			->registerArchivator( STRINGIZE_STRING_LOCAL( m_serviceProvider, "zip" ), new FactorableUnique<ArchivatorZip>() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ZipPlugin::_finalize()
	{
		FILE_SERVICE(m_serviceProvider)
			->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "zip") );

		ARCHIVE_SERVICE(m_serviceProvider)
			->unregisterArchivator( STRINGIZE_STRING_LOCAL(m_serviceProvider, "zip") );
	}
}