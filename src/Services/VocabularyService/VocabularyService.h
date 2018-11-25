#pragma once

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Hashtable.h"

#include "Config/Vector.h"

namespace Mengine
{
    class VocabularyService
        : public ServiceBase<VocabularyServiceInterface>
    {
    public:
        VocabularyService();
        ~VocabularyService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        bool setFactorable( const ConstString & _category, const ConstString & _prototype, const MixinPtr & _factorable ) override;
        bool removeFactorable( const ConstString & _category, const ConstString & _prototype ) override;
        MixinPointer getFactorable( const ConstString & _category, const ConstString & _prototype ) const override;

    protected:
        struct CategoryKey
        {
            ConstString category;
            ConstString prototype;

            bool operator == ( const CategoryKey & _key ) const
            {
                return category == _key.category && prototype == _key.prototype;
            }
        };

        struct CategoryKeyHashgen
        {
            HashType operator() ( const CategoryKey & _key ) const
            {
                return _key.category.hash() + _key.prototype.hash();
            }
        };

        typedef Hashtable<CategoryKey, MixinPtr, CategoryKeyHashgen> HashtableMixins;
        HashtableMixins m_mixins;
    };
}