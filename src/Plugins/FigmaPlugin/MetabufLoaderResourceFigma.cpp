#include "MetabufLoaderResourceFigma.h"

#include "ResourceFigma.h"

#include "Metacode/Metacode.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceFigma::MetabufLoaderResourceFigma()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceFigma::~MetabufLoaderResourceFigma()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceFigma::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceFigma * resource = _factorable.getT<ResourceFigma *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceFigma * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceFigma *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );
        metadata->getm_File_NoExist( content.get(), &ContentInterface::setValidNoExist );
        metadata->getm_Sidecar_Path( resource, &ResourceFigma::setSidecarPath );

        ConstString dataflowType = STRINGIZE_STRING_LOCAL( "figma" );
        metadata->get_File_Dataflow( &dataflowType );

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), dataflowType );

        MENGINE_ASSERTION_MEMORY_PANIC( dataflow, "resource '%s' invalid get dataflow '%s'"
            , resource->getName().c_str()
            , dataflowType.c_str()
        );

        content->setDataflow( dataflow );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
