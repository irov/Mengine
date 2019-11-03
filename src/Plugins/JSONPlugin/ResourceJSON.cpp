#include "ResourceJSON.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
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
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();
        const FilePath & filePath = this->getFilePath();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, false );

        JSONStorageInterfacePtr storage = JSON_SERVICE()
            ->loadJSON( stream, MENGINE_DOCUMENT_FACTORABLE );
        
        if( storage == nullptr )
        {
            LOGGER_ERROR( "invalid load json '%s'"
                , filePath.c_str()
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
}