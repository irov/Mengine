#include "ResourceDazzleEffect.h"

#include "Kernel/Dataflow.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceDazzleEffect::ResourceDazzleEffect()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceDazzleEffect::~ResourceDazzleEffect()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const DataInterfacePtr & ResourceDazzleEffect::getData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceDazzleEffect::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        const FilePath & filePath = content->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "dazzle effect resource '%s' group '%s' don`t set file path"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const DataflowInterfacePtr & dataflow = content->getDataflow();

        DataflowContext context;
        context.filePath = filePath;

        DazzleDataInterfacePtr data = Helper::getDataflow( fileGroup, filePath, dataflow, &context, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "resource '%s' group '%s' invalid compile data"
            , this->getName().c_str()
            , this->getGroupName().c_str()
        );

        if( data->acquire() == false )
        {
            return false;
        }

        m_data = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceDazzleEffect::_release()
    {
        m_data->release();
        m_data = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}