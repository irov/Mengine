#include "FETextFontEffectFile.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Dataflow.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FETextFontEffectFile::FETextFontEffectFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FETextFontEffectFile::~FETextFontEffectFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectFile::_compile()
    {
        DataflowInterfacePtr dataflowFE = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );

        const ContentInterface * content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        FEDataInterfacePtr data = Helper::getDataflow( fileGroup, filePath, dataflowFE, nullptr, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data );

        fe_bundle * bundle = data->getFEBundle();

        if( this->compileFEBundle( bundle ) == false )
        {
            LOGGER_ERROR( "invalid compile text font effect '%s:%s'"
                , fileGroup->getName().c_str()
                , filePath.c_str()
            );

            return false;
        }

        m_data = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FETextFontEffectFile::_release()
    {
        m_data = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectFile::_prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( dataflow );

        const ContentInterface * content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        DataflowContext context;
        context.filePath = filePath;

        if( PREFETCHER_SERVICE()
            ->prefetchData( fileGroup, filePath, dataflow, &context, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectFile::_unfetch()
    {
        const ContentInterface * content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        if( PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectFile::isValid() const
    {
        DataflowInterfacePtr dataflowFE = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );

        const ContentInterface * content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        FEDataInterfacePtr data = Helper::getDataflow( fileGroup, filePath, dataflowFE, nullptr, MENGINE_DOCUMENT_FACTORABLE );

        if( data == nullptr )
        {
            return false;
        }

        fe_bundle * ttfFEBundle = data->getFEBundle();

        if( ttfFEBundle == nullptr )
        {
            return false;
        }

        fe_effect * ttfFEEffect = fe_bundle_get_effect_by_name( ttfFEBundle, m_effectName.c_str() );

        if( ttfFEEffect == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
