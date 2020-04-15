#pragma once

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Hashtable2.h"
#include "Kernel/Vector.h"

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
        FactorablePointer generatePrototype( const ConstString & _category, const ConstString & _prototype, const DocumentPtr & _doc ) override;

    public:
        void foreachGenerators( const LambdaPrototypeGenerator & _lambda ) const override;

    protected:
        typedef Hashtable2<ConstString, ConstString, PrototypeGeneratorInterfacePtr> HashtablePrototypes;
        HashtablePrototypes m_generators;
    };
}