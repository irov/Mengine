#include "JSONLoaderResourceFigma.h"

#include "ResourceFigma.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/JSONDataHelper.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceFigma::JSONLoaderResourceFigma()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceFigma::~JSONLoaderResourceFigma()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceFigma::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceFigma * resource = _factorable.getT<ResourceFigma *>();

        const ContentInterfacePtr & content = resource->getContent();

        FilePath filePath;
        if( Helper::getJSONMemberFilePath( _json, "File", "Path", &filePath ) == true )
        {
            content->setFilePath( filePath );
        }

        bool noExist = false;
        if( Helper::getJSONMemberBool( _json, "File", "NoExist", &noExist ) == true )
        {
            content->setValidNoExist( noExist );
        }

        ConstString dataflowType;
        if( Helper::getJSONMemberConstString( _json, "File", "Dataflow", &dataflowType ) == false )
        {
            dataflowType = STRINGIZE_STRING_LOCAL( "figma" );
        }

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), dataflowType );

        MENGINE_ASSERTION_MEMORY_PANIC( dataflow, "resource '%s' type '%s' invalid get dataflow '%s'"
            , resource->getName().c_str()
            , resource->getType().c_str()
            , dataflowType.c_str()
        );

        content->setDataflow( dataflow );

        FilePath sidecarPath;
        if( Helper::getJSONMemberFilePath( _json, "Sidecar", "Path", &sidecarPath ) == true )
        {
            resource->setSidecarPath( sidecarPath );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
