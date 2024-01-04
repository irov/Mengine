#include "JSONService.h"

#include "Kernel/JSONHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( JSONService, Mengine::JSONService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONService::JSONService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONService::~JSONService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONService::load( const ContentInterfacePtr & _content, jpp::object * const _obj, const DocumentInterfacePtr & _doc )
    {
        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();
        const FilePath & filePath = _content->getFilePath();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, _doc );

        if( stream == nullptr )
        {
            return false;
        }

        jpp::object json = Helper::loadJSONStream( stream, _doc );

        if( json.invalid() == true )
        {
            return false;
        }
        
        *_obj = json;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}