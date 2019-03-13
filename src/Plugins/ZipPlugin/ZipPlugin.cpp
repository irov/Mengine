#include "ZipPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

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
    ZipPlugin::~ZipPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipPlugin::_initializePlugin()
    {
        VOCABULARY_SET( Factory, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "zip" ), Helper::makeFactoryDefault<FileGroupZip>() );
        VOCABULARY_SET( ArchivatorInterface, STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "zip" ), Helper::makeFactorableUnique<ArchivatorZip>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ZipPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "zip" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "zip" ) );
    }
}