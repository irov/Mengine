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
		virtual bool initialize( const ConstString & _category, const ConstString & _prototype ) = 0;

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
        virtual void visit( const ConstString & _category, const ConstString & _type, const PrototypeGeneratorInterfacePtr & _factory ) = 0;
    };
	//////////////////////////////////////////////////////////////////////////
    class PrototypeServiceInterface
		: public ServiceInterface
    {
        SERVICE_DECLARE("PrototypeService")

    public:
        virtual bool addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator ) = 0;
        virtual bool removePrototype(const ConstString & _category, const ConstString & _prototype) = 0;
        virtual bool hasPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterfacePtr & _generator ) const = 0;

    public:
        virtual PointerFactorable generatePrototype( const ConstString & _category, const ConstString & _prototype ) = 0;

    public:
        virtual void visitGenerators( VisitorPrototypeGenerator * _visitor ) const = 0;
    };
}

#   define PROTOTYPE_SERVICE()\
	((PrototypeServiceInterface *)SERVICE_GET(Mengine::PrototypeServiceInterface))
    

