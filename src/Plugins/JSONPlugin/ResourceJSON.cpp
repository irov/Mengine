#include "ResourceJSON.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionJSONInvalid.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/JSONHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceJSON::ResourceJSON()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceJSON::~ResourceJSON()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const jpp::object & ResourceJSON::getJSON() const
    {
        return m_json;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceJSON::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        jpp::object json = Helper::loadJSONStream( stream, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_JSON_INVALID( json, "invalid load json '%s'"
            , this->getContent()->getFilePath().c_str()
        );

        m_json = json;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceJSON::_release()
    {
        m_json = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}