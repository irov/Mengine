#include "ZipPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/ArchiveServiceInterface.h"

#include "FileGroupZip.h"
#include "ArchivatorZip.h"

#include "Kernel/FactoryDefault.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Zip, Mengine::ZipPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ZipPlugin::ZipPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipPlugin::_initialize()
    {
        FILE_SERVICE()
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( "zip" ), new FactoryDefault<FileGroupZip>() );

        ARCHIVE_SERVICE()
            ->registerArchivator( STRINGIZE_STRING_LOCAL( "zip" ), new FactorableUnique<ArchivatorZip>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ZipPlugin::_finalize()
    {
        FILE_SERVICE()
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( "zip" ) );

        ARCHIVE_SERVICE()
            ->unregisterArchivator( STRINGIZE_STRING_LOCAL( "zip" ) );
    }
}