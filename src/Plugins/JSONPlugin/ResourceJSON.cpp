#include "ResourceJSON.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
    const jpp::object & ResourceJSON::getJSON() const
    {
        return m_json;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceJSON::_compile()
    {
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();
        const FilePath & filePath = this->getFilePath();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, false );

        jpp::object json;
        if( JSON_SERVICE()
            ->loadJSON( stream, &json, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "invalid load json '%s'"
                , filePath.c_str()
            );

            return false;
        }

        m_json = json;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceJSON::_release()
    {
        m_json = jpp::detail::invalid;
    }
}