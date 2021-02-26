#include "ResourceJSON.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"

#include "jpp/jpp.hpp"

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
    const JSONStorageInterfacePtr & ResourceJSON::getJSONStorage() const
    {
        return m_storage;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceJSON::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        JSONStorageInterfacePtr storage = JSON_SERVICE()
            ->loadJSONStream( stream, MENGINE_DOCUMENT_FACTORABLE );
        
        if( storage == nullptr )
        {
            LOGGER_ERROR( "invalid load json '%s'"
                , this->getContent()->getFilePath().c_str()
            );

            return false;
        }

        m_storage = storage;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceJSON::_release()
    {
        m_storage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}