#include "FontEffectFile.h"

#include "Kernel/Dataflow.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/PrefetcherHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FontEffectFile::FontEffectFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectFile::~FontEffectFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectFile::setContent( const ContentInterfacePtr & _content )
    {
        FontEffectBase::setContent( _content );

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "fontEffect" ) );

        _content->setDataflow( dataflow );
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectFile::setEffectName( const ConstString & _effectName )
    {
        m_effectName = _effectName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & FontEffectFile::getEffectName() const
    {
        return m_effectName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectFile::isValid() const
    {
        const ContentInterfacePtr & content = this->getContent();

        if( content == nullptr )
        {
            return false;
        }

        const FilePath & filePath = content->getFilePath();

        DataflowContext context;
        context.filePath = filePath;

        FontEffectDataInterfacePtr data = Helper::getDataflow( content, &context, MENGINE_DOCUMENT_FACTORABLE );

        if( data == nullptr )
        {
            return false;
        }

        if( data->findEffect( m_effectName, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectFile::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        const FilePath & filePath = content->getFilePath();

        DataflowContext context;
        context.filePath = filePath;

        FontEffectDataInterfacePtr data = Helper::getDataflow( content, &context, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "invalid get dataflow '%s'"
            , Helper::getContentFullPath( content ).c_str()
        );

        FontEffectDesc desc;
        uint32_t sample;
        if( data->findEffect( m_effectName, &desc, &sample ) == false )
        {
            LOGGER_ERROR( "font effect file '%s' not found effect '%s'"
                , Helper::getContentFullPath( content ).c_str()
                , m_effectName.c_str()
            );

            return false;
        }

        this->applySampleHint_( sample );

        if( this->setupDesc( desc ) == false )
        {
            LOGGER_ERROR( "font effect file '%s' invalid effect '%s'"
                , Helper::getContentFullPath( content ).c_str()
                , m_effectName.c_str()
            );

            return false;
        }

        if( FontEffectBase::_compile() == false )
        {
            return false;
        }

        m_data = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectFile::_release()
    {
        FontEffectBase::_release();

        m_data = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectFile::_prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        const ContentInterfacePtr & content = this->getContent();

        const FilePath & filePath = content->getFilePath();

        DataflowContext context;
        context.filePath = filePath;

        if( Helper::prefetchData( content, &context, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectFile::_unfetch()
    {
        const ContentInterfacePtr & content = this->getContent();

        if( Helper::unfetch( content ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
