#	include "MarmaladeFileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "MarmaladeFileGroupDirectory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_DECLARE( MarmaladeFileGroupPlugin, Menge::MarmaladeFileGroupPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeFileGroupPlugin::MarmaladeFileGroupPlugin()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeFileGroupPlugin::initialize( ServiceProviderInterface * _provider )
	{
        m_serviceProvider = _provider;
	
		FILE_SERVICE(m_serviceProvider)
			->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dir" ), new FactorableUnique<FactoryDefault<MarmaladeFileGroupDirectory> >() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeFileGroupPlugin::finalize()
	{
		FILE_SERVICE(m_serviceProvider)
			->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir") );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeFileGroupPlugin::destroy()
	{
		delete this;
	}
}