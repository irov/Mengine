#pragma once

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Hashtable.h"

#ifndef MENGINE_MASTER_RELEASE
#   include "Config/Map.h"
#endif

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
        bool setFactorable( const ConstString & _category, const ConstString & _type, const MixinPtr & _factorable ) override;
        MixinPointer removeFactorable( const ConstString & _category, const ConstString & _type ) override;
        MixinPointer getFactorable( const ConstString & _category, const ConstString & _type ) const override;
        bool hasFactorable( const ConstString & _category, const ConstString & _type ) const override;

    protected:
        void foreachVocabulary( const ConstString & _category, const LambdaVocabulary & _lambda ) const override;

    protected:
        struct CategoryKey
        {
            ConstString category;
            ConstString type;

            bool operator == ( const CategoryKey & _key ) const
            {
                return category == _key.category && type == _key.type;
            }
        };

        struct CategoryKeyHashgen
        {
            HashType operator() ( const CategoryKey & _key ) const
            {
                return _key.category.hash() + _key.type.hash();
            }
        };

        typedef Hashtable<CategoryKey, MixinPtr, CategoryKeyHashgen> HashtableMixins;
        HashtableMixins m_mixins;
    };
}