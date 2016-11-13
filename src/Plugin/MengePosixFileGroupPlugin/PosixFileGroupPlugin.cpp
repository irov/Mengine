#	include "PosixFileGroupPlugin.h"

#   include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "PosixFileGroupDirectory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengePosixFileGroup, Menge::PosixFileGroupPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PosixFileGroupPlugin::PosixFileGroupPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileGroupPlugin::_initialize()
	{
        FILE_SERVICE(m_serviceProvider)
            ->registerFileGroupFactory(STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir"), new FactorableUnique<FactoryDefault<PosixFileGroupDirectory> >());

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixFileGroupPlugin::_finalize()
	{
        FILE_SERVICE(m_serviceProvider)
            ->unregisterFileGroupFactory(STRINGIZE_STRING_LOCAL(m_serviceProvider, "dir"));
	}
}