#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/FactorablePointer.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PrototypeGeneratorInterface
        : public ServantInterface
    {
    public:
        virtual void setCategory( const ConstString & _category ) = 0;
        virtual const ConstString & getCategory() const = 0;
        virtual void setPrototype( const ConstString & _prototype ) = 0;
        virtual const ConstString & getPrototype() const = 0;

    public:
        virtual bool initialize() = 0;

    public:
        virtual PointerFactorable generate() = 0;

    public:
        virtual uint32_t count() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PrototypeGeneratorInterface> PrototypeGeneratorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class VisitorPrototypeGenerator
    {
    public:
        virtual void visit( const PrototypeGeneratorInterfacePtr & _factory ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class PrototypeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PrototypeService" )

    public:
        virtual bool addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator ) = 0;
        virtual bool removePrototype( const ConstString & _category, const ConstString & _prototype ) = 0;
        virtual const PrototypeGeneratorInterfacePtr & getGenerator( const ConstString & _category, const ConstString & _prototype ) const = 0;

    public:
        virtual PointerFactorable generatePrototype( const ConstString & _category, const ConstString & _prototype ) = 0;

    public:
        virtual void visitGenerators( VisitorPrototypeGenerator * _visitor ) const = 0;
    };
}

#define PROTOTYPE_SERVICE()\
	((PrototypeServiceInterface *)SERVICE_GET(Mengine::PrototypeServiceInterface))


