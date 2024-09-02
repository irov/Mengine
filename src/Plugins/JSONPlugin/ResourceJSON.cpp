#include "ResourceJSON.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionJSONInvalid.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/ContentHelper.h"

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

        jpp::object json;
        if( JSON_SERVICE()
            ->load( content, &json, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "invalid load json '%s'"
                , Helper::getContentFullPath( this->getContent() )
            );

            return false;
        }

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