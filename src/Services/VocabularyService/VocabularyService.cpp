#include "VocabularyService.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( VocabularyService, Mengine::VocabularyService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VocabularyService::VocabularyService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VocabularyService::~VocabularyService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VocabularyService::_initializeService()
    {
        m_mixins.reserve( 1024 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VocabularyService::_finalizeService()
    {
        MENGINE_ASSERTION( m_mixins.empty() == true, "Vocabulary not empty!" );

#ifdef MENGINE_DEBUG
        for( const HashtableMixins::value_type & value : m_mixins )
        {
            const ConstString & category = value.key1;
            const ConstString & type = value.key2;

            LOGGER_ERROR( "Vocabulary category '%s' key '%s' not clear"
                , category.c_str()
                , type.c_str()
            );
        }
#endif

        m_mixins.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool VocabularyService::setFactorable( const ConstString & _category, const ConstString & _type, const MixinPtr & _factorable, const DocumentPtr & _document )
    {
        MENGINE_UNUSED( _document );

        m_mixins.emplace( _category, _type, _factorable );

        LOGGER_INFO( "vocabulary", "add vocabulary category '%s' type '%s'"
            , _category.c_str()
            , _type.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MixinPointer VocabularyService::removeFactorable( const ConstString & _category, const ConstString & _type )
    {
        MixinPtr mixin = m_mixins.erase( _category, _type );

        LOGGER_INFO( "vocabulary", "remove vocabulary category '%s' type '%s'"
            , _category.c_str()
            , _type.c_str()
        );

        return mixin;
    }
    //////////////////////////////////////////////////////////////////////////
    MixinPointer VocabularyService::getFactorable( const ConstString & _category, const ConstString & _type ) const
    {
        const MixinPtr & mixin = m_mixins.find( _category, _type );

        return mixin;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VocabularyService::hasFactorable( const ConstString & _category, const ConstString & _type ) const
    {
        if( m_mixins.find( _category, _type ) == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VocabularyService::foreachVocabulary( const ConstString & _category, const LambdaVocabulary & _lambda ) const
    {
        for( const HashtableMixins::value_type & value : m_mixins )
        {
            const ConstString & category = value.key1;
            const ConstString & type = value.key2;

            if( category != _category )
            {
                continue;
            }

            const MixinPtr & factorable = value.element;

            _lambda( type, factorable );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}