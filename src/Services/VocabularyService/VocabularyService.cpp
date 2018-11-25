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
        m_mixins.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool VocabularyService::setFactorable( const ConstString & _category, const ConstString & _prototype, const MixinPtr & _factorable )
    {
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        m_mixins.insert( key, _factorable );

        LOGGER_INFO( "add '%s:%s'"
            , _category.c_str()
            , _prototype.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VocabularyService::removeFactorable( const ConstString & _category, const ConstString & _prototype )
    {
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        m_mixins.remove( key );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MixinPointer VocabularyService::getFactorable( const ConstString & _category, const ConstString & _prototype ) const
    {
        CategoryKey key;
        key.category = _category;
        key.prototype = _prototype;

        const MixinPtr & mixin = m_mixins.find( key );

        return mixin;
    }
}