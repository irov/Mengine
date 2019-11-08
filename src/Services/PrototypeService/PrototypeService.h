#pragma once

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Hashtable.h"

#include "Config/Vector.h"

namespace Mengine
{
    class PrototypeService
        : public ServiceBase<PrototypeServiceInterface>
    {
    public:
        PrototypeService();
        ~PrototypeService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator ) override;
        bool removePrototype( const ConstString & _category, const ConstString & _prototype ) override;
        bool hasGenerator( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterfacePtr * _generator ) const override;
        const PrototypeGeneratorInterfacePtr & getGenerator( const ConstString & _category, const ConstString & _prototype ) const override;

    public:
        FactorablePointer generatePrototype( const ConstString & _category, const ConstString & _prototype, const Char * _doc ) override;

    public:
        void foreachGenerators( const LambdaPrototypeGenerator & _lambda ) const override;

    protected:
        struct CategoryKey
        {
            CategoryKey()
                : hash( ~0ULL )
            {
            }

            CategoryKey( const ConstString & _category, const ConstString & _prototype )
                : category( _category )
                , prototype( _prototype )
                , hash( _category.hash() ^ _prototype.hash() )
            {
            }

            ConstString category;
            ConstString prototype;

            HashType hash;

            bool operator == ( const CategoryKey & _key ) const
            {
                return category == _key.category && prototype == _key.prototype;
            }
        };

        struct CategoryKeyHashgen
        {
            HashType operator() ( const CategoryKey & _key ) const
            {
                return _key.hash;
            }
        };

        typedef Hashtable<CategoryKey, PrototypeGeneratorInterfacePtr, CategoryKeyHashgen> HashtablePrototypes;
        HashtablePrototypes m_generators;
    };
}