#include "FEFontEffectFile.h"

#include "Interface/PrefetcherServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Dataflow.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FEFontEffectFile::FEFontEffectFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FEFontEffectFile::~FEFontEffectFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEFontEffectFile::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        const FilePath & filePath = content->getFilePath();

        DataflowContext context;
        context.filePath = filePath;

        FEDataInterfacePtr data = Helper::getDataflow( content, &context, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data );

        fe_bundle * bundle = data->getFEBundle();

        if( this->compileFEBundle( bundle ) == false )
        {
            LOGGER_ERROR( "invalid compile text font effect '%s'"
                , Helper::getContentFullPath( content )
            );

            return false;
        }

        m_data = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FEFontEffectFile::_release()
    {
        m_data = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEFontEffectFile::_prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        const ContentInterfacePtr & content = this->getContent();

        const FilePath & filePath = content->getFilePath();

        DataflowContext context;
        context.filePath = filePath;

        if( PREFETCHER_SERVICE()
            ->prefetchData( content, &context, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEFontEffectFile::_unfetch()
    {
        const ContentInterfacePtr & content = this->getContent();

        if( PREFETCHER_SERVICE()
            ->unfetch( content ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEFontEffectFile::isValid() const
    {
        const ContentInterfacePtr & content = this->getContent();

        const FilePath & filePath = content->getFilePath();

        DataflowContext context;
        context.filePath = filePath;

        FEDataInterfacePtr data = Helper::getDataflow( content, &context, MENGINE_DOCUMENT_FACTORABLE );

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
