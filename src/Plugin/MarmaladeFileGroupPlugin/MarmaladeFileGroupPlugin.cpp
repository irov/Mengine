#	include "MarmaladeFileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "MarmaladeFileGroupDirectory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_DECLARE( MarmaladeFileGroup, Menge::MarmaladeFileGroupPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileGroupPlugin::MarmaladeFileGroupPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupPlugin::_initialize()
	{
		FILE_SERVICE(m_serviceProvider)
			->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dir" ), new FactorableUnique<FactoryDefault<MarmaladeFileGroupDirectory> >() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeFileGroupPlugin::_finalize()
	{
		FILE_SERVICE(m_serviceProvider)
			->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir") );
	}
}