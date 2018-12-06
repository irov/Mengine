#include "VocabularyService.h"

#include "Interface/ConfigServiceInterface.h"

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
        uint32_t VocabularyHashTableSize = CONFIG_VALUE( "Engine", "VocabularyHashTableSize", 1024 );

        m_mixins.reserve( VocabularyHashTableSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VocabularyService::_finalizeService()
    {
        MENGINE_ASSERTION( m_mixins.empty(), ("Vocabulary not empty!") );
        
        for( const HashtableMixins::value_type & value : m_mixins )
        {
            const CategoryKey & key = value.key;

            LOGGER_ERROR( "Vocabulary category %s key %s not clear"
                , key.category.c_str()
                , key.type.c_str()
            );
        }

        m_mixins.clear();

#ifndef MENGINE_MASTER_RELEASE
        m_mixinss.clear();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool VocabularyService::setFactorable( const ConstString & _category, const ConstString & _type, const MixinPtr & _factorable )
    {
        CategoryKey key;
        key.category = _category;
        key.type = _type;

        m_mixins.insert( key, _factorable );

#ifndef MENGINE_MASTER_RELEASE
        m_mixinss[_category][_type] = _factorable;
#endif

        LOGGER_INFO( "add '%s:%s'"
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

#ifndef MENGINE_MASTER_RELEASE
        m_mixinss[_category].erase( _type );
#endif

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
#ifndef MENGINE_MASTER_RELEASE
    //////////////////////////////////////////////////////////////////////////
    void VocabularyService::foreachFactorable( const ConstString & _category, const LambdaFactorable & _lambda ) const
    {
        MapMixinss::const_iterator it_found = m_mixinss.find( _category );

        if( it_found == m_mixinss.end() )
        {
            return;
        }

        const MapMixins & mixins = it_found->second;

        for( const MapMixins::value_type & value : mixins )
        {
            const ConstString & type = value.first;
            const MixinPtr & factorable = value.second;

            _lambda( type, factorable );
        }
    }
#endif
}