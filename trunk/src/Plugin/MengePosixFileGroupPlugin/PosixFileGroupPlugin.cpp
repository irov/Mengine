#	include "PosixFileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "PosixFileGroupDirectory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_DECLARE( MengePosixFileGroup, Menge::PosixFileGroupPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PosixFileGroupPlugin::PosixFileGroupPlugin()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupPlugin::initialize( ServiceProviderInterface * _provider )
	{
        m_serviceProvider = _provider;
	
		FILE_SERVICE(m_serviceProvider)
			->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dir" ), new FactorableUnique<FactoryDefault<PosixFileGroupDirectory> >() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixFileGroupPlugin::finalize()
	{
		FILE_SERVICE(m_serviceProvider)
			->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir") );
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixFileGroupPlugin::destroy()
	{
		delete this;
	}
}