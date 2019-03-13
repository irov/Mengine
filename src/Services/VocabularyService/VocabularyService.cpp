#include "VocabularyService.h"

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
        MENGINE_ASSERTION( m_mixins.empty(), ("Vocabulary not empty!") );

#ifdef MENGINE_DEBUG
        for( const HashtableMixins::value_type & value : m_mixins )
        {
            const CategoryKey & key = value.key;

            LOGGER_ERROR( "Vocabulary category '%s' key '%s' not clear"
                , key.category.c_str()
                , key.type.c_str()
            );
        }
#endif

        m_mixins.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool VocabularyService::setFactorable( const ConstString & _category, const ConstString & _type, const MixinPtr & _factorable )
    {
        CategoryKey key;
        key.category = _category;
        key.type = _type;

        m_mixins.insert( key, _factorable );

        LOGGER_INFO( "add vocabulary '%s:%s'"
            , _category.c_str()
            , _type.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MixinPointer VocabularyService::removeFactorable( const ConstString & _category, const ConstString & _type )
    {
        CategoryKey key;
        key.category = _category;
        key.type = _type;

        MixinPtr mixin = m_mixins.remove( key );

        LOGGER_INFO( "remove vocabulary '%s:%s'"
            , _category.c_str()
            , _type.c_str()
        );

        return mixin;
    }
    //////////////////////////////////////////////////////////////////////////
    MixinPointer VocabularyService::getFactorable( const ConstString & _category, const ConstString & _type ) const
    {
        CategoryKey key;
        key.category = _category;
        key.type = _type;

        const MixinPtr & mixin = m_mixins.find( key );

        return mixin;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VocabularyService::hasFactorable( const ConstString & _category, const ConstString & _type ) const
    {
        CategoryKey key;
        key.category = _category;
        key.type = _type;

        if( m_mixins.find( key ) == nullptr )
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
            if( value.key.category != _category )
            {
                continue;
            }

            const ConstString & type = value.key.type;
            const MixinPtr & factorable = value.element;

            _lambda( type, factorable );
        }
    }
}