#include "LZ4Plugin.h"

#include "Interface/StringizeInterface.h"

#include "ArchivatorLZ4.h"

#include "Kernel/FactorableUnique.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( LZ4, Mengine::LZ4Plugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	LZ4Plugin::LZ4Plugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool LZ4Plugin::_initialize()
	{
		ARCHIVE_SERVICE()
			->registerArchivator( STRINGIZE_STRING_LOCAL( "lz4" ), new FactorableUnique<ArchivatorLZ4>() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void LZ4Plugin::_finalize()
	{
		ARCHIVE_SERVICE()
			->unregisterArchivator( STRINGIZE_STRING_LOCAL( "lz4") );
	}
}